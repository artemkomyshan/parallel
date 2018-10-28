#ifndef DRAFT_HPP
#define DRAFT_HPP

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


#endif // DRAFT_HPP
