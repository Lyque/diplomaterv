using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Devices.Gpio;
using Windows.Devices.I2c;
using Windows.Devices.SerialCommunication;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage.Streams;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace Windows10IoTCoreOSTest
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        // Private constants
        private const int GPIOLED0      = 6;
        private const int GPIOLED1      = 13;
        private const int GPIOSW0       = 17;
        private const int GPIOSW1       = 27;
        private const int GPIOIN        = 4;
        private const int GPIOOUT       = 5;

        private const int GPIOBLERST    = 18;
        private const int GPIOADCRST   = 22;

        // ADC address
        private const byte ADC_I2C_ADDR = 0x48;
        // ADC register map
        private const byte ADC_I2C_MODE_REG         = 0x00;
        private const byte ADC_I2C_INT_CNTRL_REG    = 0x01;
        private const byte ADC_I2C_DATA0_REG        = 0x02;
        private const byte ADC_I2C_DATA1_REG        = 0x04;
        private const byte ADC_I2C_ACQ_TIME_REG     = 0x14;
        // ADC consts (modes and conversion start)
        private const byte ADC_I2C_MODE_AWAKE_CH0   = 0x80;
        private const byte ADC_I2C_MODE_AWAKE_CH1   = 0x81;
        private const byte ADC_I2C_DATA0_CONV_START = 0xC0;
        private const byte ADC_I2C_DATA1_CONV_START = 0xC1;
        // ADC consts (read)
        private const byte ADC_I2C_READ_INC         = 0x80;
        // ADC other consts
        private const byte ADC_I2C_DISABLE_INT      = 0x00;
        private const byte ADC_I2C_MAX_ACQ_TIME     = 0x1F;


        private GpioPin LED0Pin;
        private GpioPin LED1Pin;
        private GpioPin SW0Pin;
        private GpioPin SW1Pin;
        private GpioPin INPin;
        private GpioPin OUTPin;

        private GpioPin BLERstPin;
        private GpioPin ADCRstPin;

        private I2cDevice I2CAdc;
        private bool I2cInitialized = false;

        private SerialDevice bleSerial;
        private Bluegiga.BGLib bglib = new Bluegiga.BGLib();
        private DataWriter bleOutputData;
        private DataReader bleInputData;

        public object Thread { get; private set; }

        public MainPage()
        {
            this.InitializeComponent();

            this.initGPIO();
            this.initI2C();

            this.readADCPeriodically();
        }

        private void initGPIO()
        {
            // Initialize GPIOs
            GpioController gpio = GpioController.GetDefault();

            if (gpio != null)
            {
                this.LED0Pin = gpio.OpenPin(GPIOLED0);
                this.LED1Pin = gpio.OpenPin(GPIOLED1);
                this.SW0Pin = gpio.OpenPin(GPIOSW0);
                this.SW1Pin = gpio.OpenPin(GPIOSW1);
                this.INPin = gpio.OpenPin(GPIOIN);
                this.OUTPin = gpio.OpenPin(GPIOOUT);

                this.BLERstPin = gpio.OpenPin(GPIOBLERST);
                this.ADCRstPin = gpio.OpenPin(GPIOADCRST);

                // Set default value and drive mode
                // LED0
                this.LED0Pin.Write(GpioPinValue.Low);
                this.LED0Pin.SetDriveMode(GpioPinDriveMode.Output);
                // LED1
                this.LED1Pin.Write(GpioPinValue.Low);
                this.LED1Pin.SetDriveMode(GpioPinDriveMode.Output);

                // SW0
                if (this.SW0Pin.IsDriveModeSupported(GpioPinDriveMode.InputPullUp))
                    this.SW0Pin.SetDriveMode(GpioPinDriveMode.InputPullUp);
                else
                    this.SW0Pin.SetDriveMode(GpioPinDriveMode.Input);
                this.SW0Pin.ValueChanged += this.SW0Pin_ValueChanged;
                // Jelenlegi állapot jelzése
                if (this.SW0Pin.Read() == GpioPinValue.High)
                    this.SW0CheckBox.IsChecked = true;
                else
                    this.SW0CheckBox.IsChecked = false;
                // SW1
                if (this.SW1Pin.IsDriveModeSupported(GpioPinDriveMode.InputPullUp))
                    this.SW1Pin.SetDriveMode(GpioPinDriveMode.InputPullUp);
                else
                    this.SW1Pin.SetDriveMode(GpioPinDriveMode.Input);
                this.SW1Pin.ValueChanged += this.SW1Pin_ValueChanged;
                // Jelenlegi állapot jelzése
                if (this.SW1Pin.Read() == GpioPinValue.High)
                    this.SW1CheckBox.IsChecked = true;
                else
                    this.SW1CheckBox.IsChecked = false;

                // BLERst
                this.BLERstPin.Write(GpioPinValue.Low);
                this.BLERstPin.SetDriveMode(GpioPinDriveMode.Output);
                // ADCRst
                this.ADCRstPin.Write(GpioPinValue.Low);
                this.ADCRstPin.SetDriveMode(GpioPinDriveMode.Output);
            }
            else
            {
                this.ErrorValue.Text = "GPIO error!";
                this.ErrorMessage.Text = "Initialization error.";
            }
        }

        private async void initI2C()
        {
            // Initialice I2C and ADC
            var i2cSettings = new I2cConnectionSettings(ADC_I2C_ADDR);
            var i2cController = await I2cController.GetDefaultAsync();

            i2cSettings.BusSpeed = I2cBusSpeed.FastMode;

            this.I2CAdc = i2cController.GetDevice(i2cSettings);

            byte[] modeWriteBuff = new byte[] { ADC_I2C_MODE_REG, ADC_I2C_MODE_AWAKE_CH0 };
            byte[] alarmWriteBuff = new byte[] { ADC_I2C_INT_CNTRL_REG, ADC_I2C_DISABLE_INT };
            byte[] acqTimeWriteBuff = new byte[] { ADC_I2C_ACQ_TIME_REG, ADC_I2C_MAX_ACQ_TIME };

            // ADC reset elengedése, majd 1ms várakozás, mielőtt a konfigurációs regiszterekbe irunk.
            this.ADCRstPin.Write(GpioPinValue.High);
            await Task.Delay(1);

            if(this.I2CAdc !=null)
            {
                try
                {
                    this.I2CAdc.Write(modeWriteBuff);
                    this.I2CAdc.Write(alarmWriteBuff);
                    this.I2cInitialized = true;
                }
                catch (Exception ex)
                {
                    this.ErrorValue.Text = "I2C error!";
                    this.ErrorMessage.Text = ex.Message;
                }
            }
            else
            {
                this.ErrorValue.Text = "I2C error!";
                this.ErrorMessage.Text = "Initialization error.";
            }
        }

        private void LED0CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            // LED0 értékének beállitása
            this.LED0Pin.Write(GpioPinValue.High);
        }

        private void LED0CheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            // LED0 értékének beállitása
            this.LED0Pin.Write(GpioPinValue.Low);
        }

        private void LED1CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            // LED1 értékének beállitása
            this.LED1Pin.Write(GpioPinValue.High);
        }

        private void LED1CheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            // LED1 értékének beállitása
            this.LED1Pin.Write(GpioPinValue.Low);
        }

        private void SW0Pin_ValueChanged(GpioPin sender, GpioPinValueChangedEventArgs e)
        {
            // GUI elem beállitása a kapcsoló állapotának megfelelően
            if (e.Edge == GpioPinEdge.RisingEdge)
                Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
                    () =>
                    {
                        this.SW0CheckBox.IsChecked = true;
                    }
                );
            else
                Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
                    () =>
                    {
                        this.SW0CheckBox.IsChecked = false;
                    }
                );
        }

        private void SW1Pin_ValueChanged(GpioPin sender, GpioPinValueChangedEventArgs e)
        {
            // GUI elem beállitása a kapcsoló állapotának megfelelően
            if (e.Edge == GpioPinEdge.RisingEdge)
                Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
                    () =>
                    {
                        this.SW1CheckBox.IsChecked = true;
                    }
                );
            else
                Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
                    () =>
                    {
                        this.SW1CheckBox.IsChecked = false;
                    }
                );
        }

        private async void readADCPeriodically()
        {
            // Szükséges üzenetek és bufferek
            byte[] changeToCh0WriteBuff = new byte[] { ADC_I2C_MODE_REG, ADC_I2C_MODE_AWAKE_CH0 };
            byte[] changeToCh1WriteBuff = new byte[] { ADC_I2C_MODE_REG, ADC_I2C_MODE_AWAKE_CH1 };
            byte[] startConvCh0WriteBuff = new byte[] { ADC_I2C_MODE_REG, ADC_I2C_DATA0_CONV_START };
            byte[] startConvCh1WriteBuff = new byte[] { ADC_I2C_MODE_REG, ADC_I2C_DATA1_CONV_START };
            byte[] readCh0RegWriteBuff = new byte[] { ADC_I2C_DATA0_REG | ADC_I2C_READ_INC };
            byte[] readCh1RegWriteBuff = new byte[] { ADC_I2C_DATA1_REG | ADC_I2C_READ_INC };
            byte[] readTempBuff = new byte[2];
            byte[] readPotmeterBuff = new byte[2];

            while (true)
            {
                if (this.I2CAdc != null && this.I2cInitialized)
                {
                    try
                    {
                        // Hőmérő kiolvasása. A konverzió kétszer kerül inditásra, mert egyébként a két csatorna befolyásolja egymást.
                        this.I2CAdc.Write(startConvCh0WriteBuff);
                        await Task.Delay(1);
                        this.I2CAdc.Write(startConvCh0WriteBuff);
                        this.I2CAdc.WriteRead(readCh0RegWriteBuff, readTempBuff);
                        // Hőmérő adatainak feldolgozása
                        this.processLocalTemp(readTempBuff);
                        // Potmeter kiolvasása
                        this.I2CAdc.Write(startConvCh1WriteBuff);
                        await Task.Delay(1);
                        this.I2CAdc.Write(startConvCh1WriteBuff);
                        this.I2CAdc.WriteRead(readCh1RegWriteBuff, readPotmeterBuff);
                        // Potmeter adatainak feldolgozása
                        this.processPotmeter(readPotmeterBuff);
                    }
                    catch (Exception ex)
                    {
                        this.ErrorValue.Text = "I2C error!";
                        this.ErrorMessage.Text = ex.Message;
                    }
                }
                await Task.Delay(500);
            }
        }

        private void processLocalTemp(byte[] data)
        {
            const float filterCoeff = 5;

            if(data.Length == 2)
            {
                int val = checked((int)(data[0] << 4)) + checked((int)(data[1] >> 4));
                int celsValue = 300 * val / 4095 - 50;

                this.LocalTempValue.Text = celsValue.ToString() + "°C";
            }
            else
            {
                this.ErrorValue.Text = "Data error!";
                this.ErrorMessage.Text = "Local Temperature data length is not correct.";
            }
        }

        private void processPotmeter(byte[] data)
        {
            if (data.Length == 2)
            {
                int val = checked((int)(data[0] << 4)) + checked((int)(data[1] >> 4));
                float potValue = 100 * val / 4095;

                this.PotmeterValue.Text = potValue.ToString() + "%";
            }
            else
            {
                this.ErrorValue.Text = "Data error!";
                this.ErrorMessage.Text = "Local Temperature data length is not correct.";
            }
        }
    }
}

