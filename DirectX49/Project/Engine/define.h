#pragma once

#define Single(classtype) private : \
								classtype(); \
								~classtype(); \
								friend class Singleton<classtype>;\



#define DEVICE Device::GetInst()->GetDevice()
#define DEVICECONTEXT Device::GetInst()->GetDeviceContext()