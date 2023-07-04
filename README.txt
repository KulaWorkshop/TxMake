txdump     - Dump .TGI files.
txgenerate - Generate compatible texture images before TGI creation.
txmake     - Build final TGI file from a specified configuration file.

If you are having any issues running any of the tools, please check the following:
 - Only 64-bit versions of Windows are supported.
 - Make sure you have the Microsoft Runtime Redistributable to be installed - https://aka.ms/vs/17/release/vc_redist.x64.exe
 - Only .PNG images are supported.
 - The txtemplate.dat file is required to be in the same directory as the txmake tool, and should not be renamed.

If you have any more issues, feel free to post an issue in the GitHub repository - https://github.com/KulaWorkshop/TxMake/issues