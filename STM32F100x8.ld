/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

/*
 * ST32F100xB memory setup.
 */


__main_stack_size__     = 0x0200;
__process_stack_size__  = 0x0200;
 


MEMORY
{
    flash : org = 0x08000000, len = 64k
    ram : org = 0x20000000, len = 8k
}

INCLUDE rules.ld
