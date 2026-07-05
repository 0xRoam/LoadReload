# LoadReload
Shellcode Loader (based on what i did for my Master Thesis), designed for Sliver-Shellcode (smaller than 25MB....).


> [!Note]
> **Features:**
> 
> - Multibyte-XOR Decryption of Payload
> - Fetches Payload locally
> - Anti-Emulation by BusySleep-Function via KUSER_SHARED_DATA
> - RW-RX-Loop for EDR-Deconditioning / Wasting Resources for Memory-Scanners
> - Preloading of Network-DLLs against behaviour alerts like "Network Module from Stomped Module"
> - Module Overloading of Decoy-DLL chosen by Operator (currently edgehtml.dll)
> - IAT Obfuscation by API-Hashing and Dynamic API-Resolution
> - Shellcode Execution via LdrCallenclave
> 
> **References & Thanks**
> 
> I couldnt have achieved anything in this thesis without the awesome work of other people and the great support of my thesis supervisor. So I wanna thank each of these persons guiding me and helping me grow.
> - https://github.com/eversinc33
> - https://github.com/dobin
> - https://github.com/S3cur3Th1sSh1t
> - and many more...
>
> The Features listed above where inspired by:
> - https://maldevacademy.com
> - https://blog.deeb.ch/posts/how-edr-works/
> - https://github.com/dobin/SuperMega/blob/main/data/source/antiemulation/sirallocalot.c
> - https://github.com/dobin/SuperMega/blob/main/data/source/antiemulation/timeraw.c
> - https://gist.github.com/whokilleddb/ef1f8c33947f6ceb90664ce38d3dcf04
> - https://github.com/tlsbollei/KittyLoader


> [!CAUTION]
> **Disclaimer & Legal Notice**  
> This repository, **LoadReload**, and all associated code, techniques, and information are provided strictly for **educational and academic research purposes**.  
>
> This sample and its methodologies are actively used to research attack patterns, develop detection capabilities, and enhance security products.  
>
> You are required to use this knowledge and these tools **only on systems you own or have explicit, written permission to test**.  
> Any unauthorized use against systems you do not own is **illegal and strictly prohibited**.  
>
> This tool was created to advance the field of defensive cybersecurity. The author, *0xRoam*, assumes **no liability** and is not responsible for any misuse or damage caused by this software.  
>
> By accessing this repository, you acknowledge that you understand its purpose is to learn about **modern malware techniques, evasion tactics**, and ultimately to **improve our collective ability to defend against them**.
