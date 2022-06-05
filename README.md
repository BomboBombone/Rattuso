# ManualMappingTest
Started as a test to learn how to manual map DLLs and then call functions from it without using LoadLibraryA or GetProcAddress.

## Final objective:
1. Main executable will try to escalate privileges using a UAC bypass (taken and modified from UACme project)
2. Main executable will try to open explorer.exe (but any process could work as long as you have access to it) and manual map a DLL inside of it. The DLL  
will be taken from the main executable own image (check out Objectify project on my github).
3. DLL will then attempt to write to disk a Windows service (.NET obfuscated as heavily as possible if you want to be extra safe) which contains the main "Shell manager" and start it   
4. Main executable will attempt to disable Windows Defender
5. Main executable will attempt to delete useless files after loading everything needed inside the system
6. Service will unpack/download the shell and run it
7. (Optional) Shell will download/unpack an information scraper and run it
8. (Optional) Shell will download/unpack a silent miner and run it
  
# Service needed characteristics:  
1. Service must be obfuscated, either using custom or commercial .NET obfuscators due to not many people being able to reverse both .NET and native assembly,  
especially if heavily obfuscated :D  
2. It will need an internal Shell Manager class which holds inside its own image the shell to load into memory  
3. Shell Manager will need methods to load the shell, check if it's still running, etc.  
4. (Extra point maybe?) Have an internal shell that activates only if the native shell for some reason can't be loaded in memory  

# Shell needed characteristics:  
1. Be able to use cmd or powershell as needed  
## Optional points:  
1. Be able to check if there was a modification in the hosts file/firewall to block network traffic
2. Automatically attempt to unpack/download the Information Scraper and run it
  
# Info scraper:
1. Keylogger (Also log app foreground switching)
2. Mozilla/Chrome/Brave password scraper
3. Discord token logger  
  
## Additional points to take into consideration for the future:
At this point we will have a persistent and elevated reverse shell inside the target system, which starts itself up at each boot operation of the system.  
One could nonetheless want more, and what could be better than a persistent root-kit?

### How to insert a persistent root-kit in the system
After crafting a malicious driver the steps are pretty straighforward:
1. Find a signed and vulnerable driver (Not any driver works though, as most people have anti cheat systems in place which will not start the game if a specific driver 
is present in the system, which could lead to the person try to reset the computer or in general doubt that something could be happening in the system)
2. Use the vulnerable driver to manual map your malicious driver
3. Use a simple loader to do this at startup by placing it inside the "Startup" folder using your privileged shell

### Why should one waste so much time for so little improvement?  
Because this whole project is done for fun, so why not. Also having access to kernel space means a couple things:  
1. Completely undetectable, and also could hide any malicious process from the OS by editing the EPROCESS linked list of known processes  
2. Have access to kernel structures which could be useful for someone with the right ideas in mind  
3. What else do you need really?  

## Can I use this for my own purposes?  
Why the fuck not, I have nothing to do with anything you do, this is a project made just for fun, but beware that you will need some packer/obfuscator to avoid  
AV signature detection mechanisms if this project gets big enough. Have fun I guess!  
