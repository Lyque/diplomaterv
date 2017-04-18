using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Devices.Enumeration;
using Windows.Devices.Gpio;
using Windows.Devices.I2c;
using Windows.Devices.SerialCommunication;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
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

        // BLE SensorTag Address
        private const byte SENSORTAG_ADDRESS_0      = 0x01;
        private const byte SENSORTAG_ADDRESS_1      = 0x47;
        private const byte SENSORTAG_ADDRESS_2      = 0xC1;
        private const byte SENSORTAG_ADDRESS_3      = 0x89;
        private const byte SENSORTAG_ADDRESS_4      = 0x71;
        private const byte SENSORTAG_ADDRESS_5      = 0x24;
        // BLE Handles
        //  Temperature
        private const byte BLETEMPDATAHNDL          = 0x0024;
        private const byte BLETEMPCONFIGHNDL        = 0x0027;
        private const byte BLETEMPPERIODHNDL        = 0x0029;
        //  Light
        private const byte BLELIGHTDATAHNDL         = 0x0044;
        private const byte BLELIGHTCONFIGHNDL       = 0x0047;
        private const byte BLELIGHTPERIODHNDL       = 0x0049;
        // BLE other consts
        private const int BLECONNINTERVALMIN        = 6;
        private const int BLECINNINTERVANMAX        = 3200;
        private const int BLECONNTIMEOUT            = 1000;
        private const int BLECONNSLAVELATENCY       = 0;
        // Serial communication baudrate
        private const int BLEBAUDRATE               = 115200;


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
        private System.Threading.SemaphoreSlim bleSemaphore = new System.Threading.SemaphoreSlim(0, 1);
        private byte bleConnectionHndl;
        private bool SerialInitialized = false;
        private DataReader bleInputData;

        private StorageFile logFile;
        private string fileName = "logfile.txt";
        private List<string> stringQueue;

        public object Thread { get; private set; }

        public MainPage()
        {
            this.InitializeComponent();

            this.initLog();
            this.initGPIO();
            this.initI2C();
            this.initBLE();

            this.dumpToFile();
            this.readADCPeriodically();
            this.readBLEPeriodically();
            this.SerialListen();
        }

        private async void initLog()
        {
            try
            {
                this.stringQueue = new List<string>();
                this.logFile = await Windows.Storage.ApplicationData.Current.LocalFolder.CreateFileAsync(this.fileName, CreationCollisionOption.OpenIfExists);
            }
            catch (Exception ex)
            {
                this.ErrorValue.Text = "File error!";
                this.ErrorMessage.Text = ex.Message;
            }
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

                // In
                if (this.INPin.IsDriveModeSupported(GpioPinDriveMode.InputPullUp))
                    this.INPin.SetDriveMode(GpioPinDriveMode.InputPullUp);
                else
                    this.INPin.SetDriveMode(GpioPinDriveMode.Input);
                this.INPin.ValueChanged += this.INPin_ValueChanged;

                // Out
                this.OUTPin.SetDriveMode(GpioPinDriveMode.Output);
                this.OUTPin.Write(this.INPin.Read());

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


        private async void initBLE()
        {
            string aqs = SerialDevice.GetDeviceSelector("UART0");
            var dis = await DeviceInformation.FindAllAsync(aqs);

            this.bleSerial = await SerialDevice.FromIdAsync(dis[0].Id);

            if (this.bleSerial != null)
            {
                this.bleSerial.WriteTimeout = TimeSpan.FromMilliseconds(1);
                this.bleSerial.ReadTimeout = TimeSpan.FromMilliseconds(1);
                this.bleSerial.BaudRate = BLEBAUDRATE;
                this.bleSerial.Parity = SerialParity.None;
                this.bleSerial.StopBits = SerialStopBitCount.One;
                this.bleSerial.DataBits = 8;
                this.bleSerial.Handshake = SerialHandshake.None;

                this.bleInputData = new DataReader(this.bleSerial.InputStream);

                // Register response callback functions
                this.bglib.BLEResponseSystemHello += new Bluegiga.BLE.Responses.System.HelloEventHandler(this.SystemHelloResponse);
                this.bglib.BLEResponseGAPConnectDirect += new Bluegiga.BLE.Responses.GAP.ConnectDirectEventHandler(this.GAPConnectDirect);

                // Register event callback functions
                this.bglib.BLEEventSystemBoot += new Bluegiga.BLE.Events.System.BootEventHandler(this.SystemBootEvent);
                this.bglib.BLEEventConnectionStatus += new Bluegiga.BLE.Events.Connection.StatusEventHandler(this.ConnectionStatusEvent);
                this.bglib.BLEEventATTClientProcedureCompleted += new Bluegiga.BLE.Events.ATTClient.ProcedureCompletedEventHandler(this.ATTClientProcedureCompletedEvent);
                this.bglib.BLEEventATTClientAttributeValue += new Bluegiga.BLE.Events.ATTClient.AttributeValueEventHandler(this.ATTClientAttributeValueEvent);

                this.BLERstPin.Write(GpioPinValue.High);
                await Task.Delay(1);
                this.SerialInitialized = true;
            }
            else
            {
                this.ErrorValue.Text = "Serial Port error!";
                this.ErrorMessage.Text = "Initialization error.";
            }
        }

        private void LED0CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            // LED0 értékének beállitása
            this.LED0Pin.Write(GpioPinValue.High);
            this.stringQueue.Add("LED0 HIGH" + Environment.NewLine);
        }

        private void LED0CheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            // LED0 értékének beállitása
            this.LED0Pin.Write(GpioPinValue.Low);
            this.stringQueue.Add("LED0 LOW" + Environment.NewLine);
        }

        private void LED1CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            // LED1 értékének beállitása
            this.LED1Pin.Write(GpioPinValue.High);
            this.stringQueue.Add("LED1 HIGH" + Environment.NewLine);
        }

        private void LED1CheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            // LED1 értékének beállitása
            this.LED1Pin.Write(GpioPinValue.Low);
            this.stringQueue.Add("LED1 LOW" + Environment.NewLine);
        }

        private void SW0Pin_ValueChanged(GpioPin sender, GpioPinValueChangedEventArgs e)
        {
            // GUI elem beállitása a kapcsoló állapotának megfelelően
            if (e.Edge == GpioPinEdge.RisingEdge)
                Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
                    () =>
                    {
                        this.SW0CheckBox.IsChecked = true;
                        this.stringQueue.Add("SW0 HIGH" + Environment.NewLine);
                    }
                );
            else
                Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
                    () =>
                    {
                        this.SW0CheckBox.IsChecked = false;
                        this.stringQueue.Add("SW0 LOW" + Environment.NewLine);
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
                        this.stringQueue.Add("SW1 HIGH" + Environment.NewLine);
                    }
                );
            else
                Windows.ApplicationModel.Core.CoreApplication.MainView.CoreWindow.Dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal,
                    () =>
                    {
                        this.SW1CheckBox.IsChecked = false;
                        this.stringQueue.Add("SW1 LOW" + Environment.NewLine);
                    }
                );
        }

        private void INPin_ValueChanged(GpioPin sender, GpioPinValueChangedEventArgs e)
        {
            this.OUTPin.Write(this.INPin.Read());
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

        private async void readBLEPeriodically()
        {
            bool rsp;

            while (true)
            {
                await Task.Delay(1000);
                if (this.bleSerial != null && this.SerialInitialized)
                {
                    try
                    {
                        do
                        {
                            await bglib.SendCommandAsync(this.bleSerial, bglib.BLECommandSystemReset(0));
                            rsp = await this.bleSemaphore.WaitAsync(2000);
                        } while (!rsp);

                        if (rsp)
                        {
                            await bglib.SendCommandAsync(this.bleSerial, bglib.BLECommandSystemHello());
                            rsp = await this.bleSemaphore.WaitAsync(2000);
                        }
                        else
                            this.ErrorMessage.Text = "Semaphore timeout";

                        if (rsp)
                        {
                            byte[] bleAddress = new byte[] { SENSORTAG_ADDRESS_0, SENSORTAG_ADDRESS_1, SENSORTAG_ADDRESS_2, SENSORTAG_ADDRESS_3, SENSORTAG_ADDRESS_4, SENSORTAG_ADDRESS_5 };
                            await bglib.SendCommandAsync(this.bleSerial, bglib.BLECommandGAPConnectDirect(bleAddress, 0, BLECONNINTERVALMIN, BLECINNINTERVANMAX, BLECONNTIMEOUT, BLECONNSLAVELATENCY));
                            rsp = await this.bleSemaphore.WaitAsync(2000);
                        }
                        else
                            this.ErrorMessage.Text = "Semaphore timeout";

                        // Config temperature sensor period
                        if (rsp)
                        {
                            byte[] temperaturePeriod = new byte[] { 100 };
                            await bglib.SendCommandAsync(this.bleSerial, bglib.BLECommandATTClientAttributeWrite(this.bleConnectionHndl, BLETEMPPERIODHNDL, temperaturePeriod));
                            rsp = await this.bleSemaphore.WaitAsync(2000);
                        }
                        else
                            this.ErrorMessage.Text = "Semaphore timeout";

                        // Config light sensor period
                        if (rsp)
                        {
                            byte[] lightPeriod = new byte[] { 100 };
                            await bglib.SendCommandAsync(this.bleSerial, bglib.BLECommandATTClientAttributeWrite(this.bleConnectionHndl, BLELIGHTPERIODHNDL, lightPeriod));
                            rsp = await this.bleSemaphore.WaitAsync(2000);
                        }
                        else
                            this.ErrorMessage.Text = "Semaphore timeout";

                        // Start temperature measurement
                        if (rsp)
                        {
                            byte[] temperatureConfig = new byte[] { 0x01 };
                            await bglib.SendCommandAsync(this.bleSerial, bglib.BLECommandATTClientAttributeWrite(this.bleConnectionHndl, BLETEMPCONFIGHNDL, temperatureConfig));
                            rsp = await this.bleSemaphore.WaitAsync(2000);
                        }
                        else
                            this.ErrorMessage.Text = "Semaphore timeout";

                        // Start light measurement
                        if (rsp)
                        {
                            byte[] lightConfig = new byte[] { 0x01 };
                            await bglib.SendCommandAsync(this.bleSerial, bglib.BLECommandATTClientAttributeWrite(this.bleConnectionHndl, BLELIGHTCONFIGHNDL, lightConfig));
                            rsp = await this.bleSemaphore.WaitAsync(2000);
                        }
                        else
                            this.ErrorMessage.Text = "Semaphore timeout";

                        while(rsp)
                        {
                            await Task.Delay(1000);
                            // Read temperature data
                            if (rsp)
                            {
                                await bglib.SendCommandAsync(this.bleSerial, bglib.BLECommandATTClientReadByHandle(this.bleConnectionHndl, BLETEMPDATAHNDL));
                                rsp = await this.bleSemaphore.WaitAsync(2000);
                            }
                            else
                                this.ErrorMessage.Text = "Semaphore timeout";

                            // Read light data
                            if (rsp)
                            {
                                await bglib.SendCommandAsync(this.bleSerial, bglib.BLECommandATTClientReadByHandle(this.bleConnectionHndl, BLELIGHTDATAHNDL));
                                rsp = await this.bleSemaphore.WaitAsync(2000);
                            }
                            else
                                this.ErrorMessage.Text = "Semaphore timeout";
                        }

                        await bglib.SendCommandAsync(this.bleSerial, bglib.BLECommandConnectionDisconnect(this.bleConnectionHndl));
                    }
                    catch (Exception ex)
                    {
                        this.ErrorValue.Text = "BGLib error!";
                        this.ErrorMessage.Text = ex.Message;
                    }
                }
            }
        }

        private void processLocalTemp(byte[] data)
        {
            if(data.Length == 2)
            {
                int val = checked((int)(data[0] << 4 | data[1] >> 4));
                int celsValue = 300 * val / 4095 - 50;

                this.LocalTempValue.Text = celsValue.ToString() + "°C";

                this.stringQueue.Add("Local temp: " + celsValue.ToString() + "°C" + Environment.NewLine);
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
                int val = checked((int)(data[0] << 4 | data[1] >> 4));
                float potValue = 100 * val / 4095;

                this.PotmeterValue.Text = potValue.ToString() + "%";

                this.stringQueue.Add("Potmeter: " + potValue.ToString() + "%" + Environment.NewLine);
            }
            else
            {
                this.ErrorValue.Text = "Data error!";
                this.ErrorMessage.Text = "Potmeter data length is not correct.";
            }
        }

        // Serial port listen
        private async void SerialListen()
        {
            try
            {
                while (true)
                {
                    if (this.bleSerial != null && this.SerialInitialized)
                    {
                        await ReadSerialAsync();
                    }
                    else
                    {
                        await Task.Delay(1);
                    }
                }
            }
            catch (Exception ex)
            {
                this.ErrorValue.Text = "Serial port error!";
                this.ErrorMessage.Text = ex.Message;
            }
        }

        // Serial port read
        private async Task ReadSerialAsync()
        {
            uint ReadBufferLength = 32;
            uint bytesRead = 0;
            this.bleInputData.InputStreamOptions = InputStreamOptions.Partial;

            try
            {
                bytesRead = await this.bleInputData.LoadAsync(ReadBufferLength);

                if(bytesRead>0)
                {
                    byte[] receivedBytes = new byte[bytesRead];
                    try
                    {
                        this.bleInputData.ReadBytes(receivedBytes);
                        for(int i=0;i<receivedBytes.Length;i++)
                        {
                            bglib.Parse(receivedBytes[i]);
                        }
                    }
                    catch (Exception ex)
                    {
                        this.ErrorValue.Text = "Parse error!";
                        this.ErrorMessage.Text = ex.Message;
                    }
                }
            }
            catch (Exception ex)
            {
                this.ErrorValue.Text = "Serial read error!";
                this.ErrorMessage.Text = ex.Message;
            }
        }

        // BLE response handlers
        public void SystemHelloResponse(object sender, Bluegiga.BLE.Responses.System.HelloEventArgs e)
        {
            this.bleSemaphore.Release();
        }

        public void GAPConnectDirect(object sender, Bluegiga.BLE.Responses.GAP.ConnectDirectEventArgs e)
        {
            this.bleConnectionHndl = e.connection_handle;
        }

        // BLE event handlers
        public void SystemBootEvent(object sender, Bluegiga.BLE.Events.System.BootEventArgs e)
        {
            this.bleSemaphore.Release();
        }

        public void ConnectionStatusEvent(object sender, Bluegiga.BLE.Events.Connection.StatusEventArgs e)
        {
            if ((e.flags & 0x01) != 0) // connection_connected
            {
                this.ErrorValue.Text = "";
                this.ErrorMessage.Text = "";
                this.bleSemaphore.Release();
            }
        }

        public void ATTClientProcedureCompletedEvent(object sender, Bluegiga.BLE.Events.ATTClient.ProcedureCompletedEventArgs e)
        {
            // Ha Write parancsra érkezett, és sikeres a folyamat...
            if(e.connection == this.bleConnectionHndl)
                if(e.chrhandle == BLETEMPPERIODHNDL || e.chrhandle== BLETEMPCONFIGHNDL || e.chrhandle == BLELIGHTPERIODHNDL || e.chrhandle == BLELIGHTCONFIGHNDL)
                    if(e.result == 0)
                    {
                        this.bleSemaphore.Release();
                    }
        }

        public void ATTClientAttributeValueEvent(object sender, Bluegiga.BLE.Events.ATTClient.AttributeValueEventArgs e)
        {
            // Adatolvasás sikeresen megtörtént
            if(e.connection == this.bleConnectionHndl)
            {
                if(e.atthandle == BLETEMPDATAHNDL)
                {
                    if(e.value.Length == 4)
                    {
                        short ambTemp = (short)(e.value[1] << 8 | e.value[0]);
                        short remTemp = (short)(e.value[3] << 8 | e.value[2]);

                        float ambTemp_f = (float)(ambTemp >> 2);
                        float remTemp_f = (float)(remTemp >> 2);

                        float ambTempVal = ambTemp_f / 32;
                        float remTempVal = remTemp_f / 32;

                        this.AmbientTempValue.Text = ((int)(ambTempVal)).ToString() + "°C";
                        this.RemoteTempValue.Text = ((int)(remTempVal)).ToString() + "°C";

                        this.stringQueue.Add("Ambient temp: " + ((int)(ambTempVal)).ToString() + "°C" + Environment.NewLine);
                        this.stringQueue.Add("Remote temp: " + ((int)(remTempVal)).ToString() + "°C" + Environment.NewLine);
                    }
                }
                else if(e.atthandle == BLELIGHTDATAHNDL)
                {
                    if(e.value.Length == 2)
                    {
                        int val = checked((int)(e.value[1] << 8 | e.value[0]));
                        int exp = (val & 0xF000) >> 12;
                        int low_limit_res = (val & 0x0FFF);

                        float lightVal = (float)(0.01 * (1 << (exp)) * low_limit_res);

                        this.LightValue.Text = lightVal.ToString() + " lux";

                        this.stringQueue.Add("Light: " + lightVal.ToString() + " lux" + Environment.NewLine);
                    }
                }
            }
            this.bleSemaphore.Release();
        }

        private async void dumpToFile()
        {
            while (true)
            {
                if (this.logFile != null) 
                {
                    while (this.stringQueue.Count() > 0)
                    {
                        try
                        {
                            var dataString = this.stringQueue[0];
                            await Windows.Storage.FileIO.AppendTextAsync(this.logFile, dataString);
                            this.stringQueue.RemoveAt(0);
                        }
                        catch (Exception ex)
                        {
                            this.ErrorValue.Text = "File write error!";
                            this.ErrorMessage.Text = ex.Message;
                        }
                    }
                }
                await Task.Delay(1000);
            }
        }
    }
}

