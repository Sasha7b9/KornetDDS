#include "Menu/Pages/Pages.h"


DEF_PAGE_3(pageMain, //-V2567
           "����", "MENU",
           "", "",
           PageSignals::self,
           PageFrequencyCounter::self,
           PageService::self,
           Page::Main, nullptr, Item::FuncActive, Page::FuncEnter, FuncOnKey, FuncBeforeDraw
)

Page *PageMain::self = reinterpret_cast<Page *>(const_cast<PageBase *>(&pageMain)); //-V2567
