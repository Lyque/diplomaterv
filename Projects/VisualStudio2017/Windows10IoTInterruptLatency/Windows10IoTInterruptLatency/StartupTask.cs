using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net.Http;
using Windows.ApplicationModel.Background;
using Windows.Devices.Gpio;

// The Background Application template is documented at http://go.microsoft.com/fwlink/?LinkID=533884&clcid=0x409

namespace Windows10IoTInterruptLatency
{
    public sealed class StartupTask : IBackgroundTask
    {
        private const int GPIOIN    = 4;
        private const int GPIOOUT   = 5;

        private GpioPin INPin;
        private GpioPin OUTPin;

        private BackgroundTaskDeferral deferral;
        public void Run(IBackgroundTaskInstance taskInstance)
        {
            this.deferral = taskInstance.GetDeferral();
            GpioController gpio = GpioController.GetDefault();

            taskInstance.Canceled += new BackgroundTaskCanceledEventHandler(TaskCanceled);

            if (gpio != null)
            {
                this.INPin = gpio.OpenPin(GPIOIN);
                this.OUTPin = gpio.OpenPin(GPIOOUT);

                // In
                if (this.INPin.IsDriveModeSupported(GpioPinDriveMode.InputPullUp))
                    this.INPin.SetDriveMode(GpioPinDriveMode.InputPullUp);
                else
                    this.INPin.SetDriveMode(GpioPinDriveMode.Input);
                this.INPin.ValueChanged += this.INPin_ValueChanged;

                // Out
                this.OUTPin.SetDriveMode(GpioPinDriveMode.Output);
                this.OUTPin.Write(this.INPin.Read());
            }
            // 
            // TODO: Insert code to perform background work
            //
            // If you start any asynchronous methods here, prevent the task
            // from closing prematurely by using BackgroundTaskDeferral as
            // described in http://aka.ms/backgroundtaskdeferral
            //
        }

        private void TaskCanceled(IBackgroundTaskInstance sender,
    BackgroundTaskCancellationReason reason)
        {
            this.deferral.Complete();
        }

        private void INPin_ValueChanged(GpioPin sender, GpioPinValueChangedEventArgs e)
        {
            this.OUTPin.Write(this.INPin.Read());
        }
    }
}
