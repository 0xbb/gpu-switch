# gpu-switch
gpu-switch is an application to switch between the integrated and dedicated GPU of dual-GPU MacBook Pro models for the next reboot.

It aims to remove the need of booting into Mac OS X and running gfxCardStatus v2.2.1 to switch to the integrated card.

### Warning:
This is new code and it comes without any warranty! It's completely based on reverse engineering. Therefore use it at your own risk and don't blame us if anything breaks.

## Linux Usage:
As root you can select the GPU by running gpu-switch and **rebooting** your machine:
#### switch to the integrated GPU:
``# ./gpu-switch -i``
#### switch to the dedicated GPU:
``# ./gpu-switch -d``

## Windows 8/10 Usage:
* Windows is using the MacBook's UEFI mode ([how to check](http://blogs.technet.com/b/home_is_where_i_lay_my_head/archive/2012/10/02/how-to-check-in-windows-if-you-are-using-uefi.aspx))
* Download the [Windows Version](https://github.com/0xbb/gpu-switch/releases/download/v0/gpu-switch-windows.zip)
* Run the provided scripts by right click "Run as administrator"  and **rebooting** your machine:
  * switch to the integrated GPU:  ``integrated.bat``
  * switch to the dedicated GPU: ``dedicated.bat``

### MacBook Pro 11,3 and 11,5 notes:
By default the Intel GPU gets switched off by the MacBook Pro 11,3's (and 11,5's) EFI if you boot anything but Mac OS X.
So to use the Intel GPU, you need to trick the EFI by using the "apple_set_os" hack either with:
- rEFInd version 0.10.0 or above (recommended): http://www.rodsbooks.com/refind

  Recent versions of rEFInd have the "apple_set_os" hack built-in. You can enable it by setting the ``spoof_osx_version`` option in your `refind.conf`.

or
- apple_set_os.efi: https://github.com/0xbb/apple_set_os.efi

or
- a patched GRUB:
 - https://lists.gnu.org/archive/html/grub-devel/2013-12/msg00442.html
 - https://wiki.archlinux.org/index.php/MacBookPro11,x#Getting_the_integrated_intel_card_to_work_on_11.2C3

or
- a patched Kernel: https://www.marc.info/?l=grub-deavel&m=141586614924917&w=2

Otherwise you will end up with a powered-down integrated graphics card and a **black screen**.

## Tested Hardware:
- MacBook Pro 5,2  (Early 2009, Non-Retina)
- MacBook Pro 5,3  (Mid  2009, Non-Retina)
- MacBook Pro 8,2  (Late 2011, Non-Retina)
- MacBook Pro 9,1  (Mid  2012, Non-Retina)
- MacBook Pro 10,1 (Mid  2012, Retina)
- MacBook Pro 11,3 (Late 2013, Retina)
- MacBook Pro 11,5 (Mid 2015, Retina)

## Future Work:
- Testing
- Integrating "apple_set_os" in the Kernel or GRUB?
- Documentation
 - (So far : https://gist.github.com/0xbb/974999591da4b1b2635c)

## Troubleshooting:
If you are facing weird problems a NVRAM reset could help:
http://support.apple.com/kb/PH14222?viewlocale=en_US

(Warning: this also resets your EFI boot configuration)

## License:
```
Copyright (C) 2014-2016 Bruno Bierbaumer, Andreas Heider, Malte Bargholz
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.
```
