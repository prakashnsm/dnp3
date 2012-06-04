
#ifndef __DOT_NET_STACK_MANAGER_H_
#define __DOT_NET_STACK_MANAGER_H_

using namespace System;
using namespace DNP3::Interface;

namespace apl { namespace dnp {
	class StackManager;
}}

namespace DNPDotNet {

	public ref class DotNetStackManager
	{
		public:
			DotNetStackManager();

			
			void AddTCPClient(System::String^ name, FilterLevel level, System::UInt64 retryMs, System::String^ address, System::UInt16 port);					
			void AddTCPServer(System::String^ name, FilterLevel level, System::UInt64 retryMs, System::String^ endpoint, System::UInt16 port);

			//void AddSerial(const std::string& arName, FilterLevelDN level, System::UInt64 retryMs, SerialSettings aSerial);
						
			ICommandAcceptor^ AddMaster(	System::String^ portName,
											System::String^ stackName,	                            
											FilterLevel level,
											IDataObserver^ publisher,
											DNP3::Interface::MasterStackConfig^ config);

		private:
			apl::dnp::StackManager* pMgr;
	};
}

#endif
