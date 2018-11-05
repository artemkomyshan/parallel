/*
* Copyright (c) 2018 Parallel
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
/*
* Author: Artem Komyshan
*/

#pragma once

#include <type_traits>
#include <memory>
#include <map>
#include "containers/thread_pool.hpp"

struct IFileHandler{};

struct IProxyObject
{
   void open(){};
   void close(){};
};

template <typename T>
struct TProxyObject : public IProxyObject
{
   struct tProxHandler{} ;
   tProxHandler createProxyObjectHandler(){return tProxHandler{};}

   static void sendEvent(tProxHandler ph, T v){}
};

class CAsyncPcl
{
   using tId = uint64_t;

   std::map<tId, std::unique_ptr<IProxyObject>> mProxies; // mb unordered map

   tId idGenerator()
   {
      static tId id = 0;
      return ++id;
   }

public:

   template <typename FuncWork, typename FuncResponce>
   void fileAccessor(std::string key, FuncWork work, FuncResponce resp)
   {
      using ResultOfWork = typename std::result_of<FuncWork(IFileHandler)>::type;
      template <typename R>
      struct TResponse
      {
         R result;
         uint64_t id;//some id
      };
      using tResponse =  TResponse<ResultOfWork>;
      using tProxy = TProxyObject< tResponse >;

      auto id = idGenerator();

      auto responceProcesor = [resp](tResponse& r)
      {
         resp(r.result);
         proxies.erase(r.id);
      };

      auto proxy = std::make_unique<tProxy>(responceProcesor);
      auto proxyHandler = proxy->createProxyObjectHandler;
      proxies.emplace(id, std::move(proxy));

      thread_pool::submit([key, work, id, proxyHandler]()// ned move to function
      {
         IFileHandler file{key};
         file.open();
         auto result = work(file);
         file.close();

         if (auto proxy = weak_proxy.lock())
         {
            tProxy::sendEvent(proxyHandler, {result, id});
         }
      });
   }
};
