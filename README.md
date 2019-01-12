Name: ML_TM4C_Examples\
Author: Marquez Jones\
Desc: \
  This is a set of code examples I created for the TIVA processors.\
  All examples are designed for the TI TM4C123GH6PM MCU which is the\ 
  exact processor on the TIVA dev boards.\

How to Use:\
  Just add all c and h files into a CCS project. Ensure TivaWare library\
  is linked to the project.\

Hardware Notes: \
  All examples were written using for the launchpad likewise, wiring\
  instructions will be included in the code files. I'll make separate\
  READMEs if I feel the need to.\

Code Composer Notes: \
  CCS defaults to using C89(an older version of C),but\
  for the examples, I switched to C99. If you try to run \
  the examples in CCS without switching to C89, it simply\
  won't compile as there are slight syntax differences between\
  the two versions of C. \
  Code Composer can be installed here:\
  http://www.ti.com/tool/CCSTUDIO\
                                   
TivaWare Notes: \
  All examples were written using the TivaWare driver library which can be \
  installed here:\
  http://www.ti.com/tool/SW-TM4\
  This library must be further linked to you CCS project after installation.\
  Instructions here:\
  https://e2e.ti.com/support/microcontrollers/other/f/908/p/368823/1296745#1296745\
