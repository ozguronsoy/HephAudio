# Events

Events manage the callback methods and arguments for handling specific cases. 
Events are raised by calling the ``Invoke`` method or the ``()`` operator. 
The handlers are called in the order they are added.
```c++
#include <iostream>
#include <Event.h>
#include <ConsoleLogger.h>

using namespace Heph;

void Handler_1(const EventParams& eventParams);
void Handler_2(const EventParams& eventParams);
void Handler_3(const EventParams& eventParams);
void Handler_4(const EventParams& eventParams);
void Handler_5(const EventParams& eventParams);

int main()
{
    Event myEvent;

    myEvent = &Handler_1; // or SetEventHandler
    myEvent.AddEventHandler(&Handler_2);
    myEvent += &Handler_3;
    myEvent += &Handler_4;
    myEvent += &Handler_5;

    // both event args and event result can be nullptr
    // or anything that derives from these base classes.
    EventArgs myEventArgs;
    EventResult myEventResult;
    myEvent.Invoke(&myEventArgs, &myEventResult);
    //myEvent(&myEventArgs, &myEventResult);

    // stop program from exiting
    int x;
    std::cin >> x;

    return 0;
}

void Handler_1(const EventParams& eventParams)
{
    ConsoleLogger::LogDebug("Handler 1!");
}
void Handler_2(const EventParams& eventParams)
{
    ConsoleLogger::LogDebug("Handler 2!");
}
void Handler_3(const EventParams& eventParams)
{
    ConsoleLogger::LogDebug("Handler 3!");
    // indicate that event is handled
    // and stop calling the other event handlers
    eventParams.pResult->isHandled = true;
}
void Handler_4(const EventParams& eventParams)
{
    ConsoleLogger::LogDebug("Handler 4!");
}
void Handler_5(const EventParams& eventParams)
{
    ConsoleLogger::LogDebug("Handler 5!");
}
```

<br><br>
You can also set additional arguments.
```c++
#include <iostream>
#include <Event.h>
#include <ConsoleLogger.h>
#include <StringHelpers.h>

using namespace Heph;

#define MY_ARG_1_KEY "MY_ARG_1"
#define MY_ARG_2_KEY "MY_ARG_2"

int myArg1 = 20;
std::string myArg2 = "user arg!";

void Handler_1(const EventParams& eventParams);
void Handler_2(const EventParams& eventParams);

int main()
{
    Event myEvent;

    myEvent = &Handler_1; // or SetEventHandler
    myEvent += &Handler_2;

    // key-value
    // user args are passed as void*
    // and must exist while handling the event.
    myEvent.userEventArgs.Add(MY_ARG_1_KEY, &myArg1);
    myEvent.userEventArgs.Add(MY_ARG_2_KEY, &myArg2);

    EventArgs myEventArgs;
    EventResult myEventResult;
    myEvent.Invoke(&myEventArgs, &myEventResult);

    // stop program from exiting
    int x;
    std::cin >> x;

    return 0;
}

void Handler_1(const EventParams& eventParams)
{
    int myArg1 = *(int*)eventParams.userEventArgs[MY_ARG_1_KEY];
    ConsoleLogger::LogDebug("user arg 1: " + StringHelpers::ToString(myArg1));
}
void Handler_2(const EventParams& eventParams)
{
    std::string* pMyArg2 = (std::string*)eventParams.userEventArgs[MY_ARG_2_KEY];
    ConsoleLogger::LogDebug(*pMyArg2);
}
```
