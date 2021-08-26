# rdpfuzz
Tools for fuzzing RDP.

This project contains:
1. DynamoRIO that was modified to be able to attach to running processes.
2. WinAFL that was modified to use the DynamoRIO attach functionality, use background fuzzing, and interact with the rdpfuzzagent.
3. FreeRDP that was modified to send fuzzing messages. 
4. rdpfuzzagent - an agent enforcing protocol requirements, and sending messages over an RDP connection.
5. Auxiliary scripts.

## References:
For more comments, suggestions or questions, contact Or Ben-Porath ([@OrBenPorath](https://twitter.com/OrBenPorath)) and Shaked Reiner ([@ShakReiner](https://twitter.com/ShakReiner)) from CyberArk Labs.

## Certification level

![](https://img.shields.io/badge/Certification%20Level-Community-28A745?link=https://github.com/cyberark/community/blob/master/Conjur/conventions/certification-levels.md)

This repo is a **Community** level project. It's a community contributed project that **is not reviewed or supported
by CyberArk**. For more detailed information on our certification levels, see [our community guidelines](https://github.com/cyberark/community/blob/master/Conjur/conventions/certification-levels.md#community).

## Notices
Use the included third party projects only according to their license:

1. [DynamoRIO](https://github.com/DynamoRIO/dynamorio) - [BSD
license](https://github.com/DynamoRIO/dynamorio/blob/master/License.txt).
2. [WinAFL](https://github.com/googleprojectzero/winafl) - [Apache 2.0 license](https://github.com/googleprojectzero/winafl/blob/master/LICENSE).
3. [FreeRDP](https://github.com/FreeRDP/FreeRDP) - [Apache 2.0 license](https://github.com/FreeRDP/FreeRDP/blob/master/LICENSE).

## License

Copyright (c) 2020 CyberArk Software Ltd. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

For the full license text see [`LICENSE`](LICENSE).


