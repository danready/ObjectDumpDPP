all: ./ObjectCode/Main.o ./ObjectCode/LogFile.o ./ObjectCode/ConfigurationConsistence.o ./ObjectCode/X742DecodeRoutines.o ./ObjectCode/TcpUser.o ./ObjectCode/CommunicationObject.o ./ObjectCode/ApplicationSetup.o ./ObjectCode/Input.o ./ObjectCode/DigitizerFlowControl.o ./ObjectCode/DigitizerStateMachine.o ./ObjectCode/DigitizerObjectGeneric.o ./ObjectCode/RawData.o ./ObjectCode/DigitizerErrorObject.o ./ObjectCode/Analizzatore.o ./ObjectCode/AnalizzatoreUtils.o ./ObjectCode/DigitizerObject.o ./ObjectCode/ConfObject.o ./ObjectCode/ClientApplication.o ./ObjectCode/OutputModule.o ./ObjectCode/Decode.o ./ObjectCode/Decode742.o
	g++ -g -o objectdump ./ObjectCode/Main.o ./ObjectCode/LogFile.o ./ObjectCode/ConfigurationConsistence.o ./ObjectCode/X742DecodeRoutines.o ./ObjectCode/TcpUser.o ./ObjectCode/CommunicationObject.o ./ObjectCode/ApplicationSetup.o ./ObjectCode/Input.o ./ObjectCode/DigitizerFlowControl.o ./ObjectCode/DigitizerStateMachine.o ./ObjectCode/DigitizerObjectGeneric.o ./ObjectCode/RawData.o ./ObjectCode/DigitizerErrorObject.o ./ObjectCode/Analizzatore.o ./ObjectCode/OutputModule.o ./ObjectCode/AnalizzatoreUtils.o ./ObjectCode/DigitizerObject.o ./ObjectCode/ConfObject.o -lCAENDigitizer -lpthread -std=gnu++11 -pthread
	gcc ./ObjectCode/ClientApplication.o -o objectdumpclient -lpthread
	gcc ./ObjectCode/Decode.o -o ./RawData/decode -lCAENDigitizer
	g++ ./ObjectCode/Decode742.o -o ./RawData/decode742 -lCAENDigitizer `root-config --glibs`

server: ./ObjectCode/Main.o ./ObjectCode/LogFile.o ./ObjectCode/ConfigurationConsistence.o ./ObjectCode/X742DecodeRoutines.o ./ObjectCode/TcpUser.o ./ObjectCode/CommunicationObject.o ./ObjectCode/ApplicationSetup.o ./ObjectCode/Input.o ./ObjectCode/DigitizerFlowControl.o ./ObjectCode/DigitizerStateMachine.o ./ObjectCode/DigitizerObjectGeneric.o ./ObjectCode/RawData.o ./ObjectCode/DigitizerErrorObject.o ./ObjectCode/Analizzatore.o ./ObjectCode/AnalizzatoreUtils.o ./ObjectCode/DigitizerObject.o ./ObjectCode/ConfObject.o ./ObjectCode/OutputModule.o
	g++ -g -o objectdump ./ObjectCode/Main.o ./ObjectCode/LogFile.o ./ObjectCode/ConfigurationConsistence.o ./ObjectCode/X742DecodeRoutines.o ./ObjectCode/TcpUser.o ./ObjectCode/CommunicationObject.o ./ObjectCode/ApplicationSetup.o ./ObjectCode/Input.o ./ObjectCode/DigitizerFlowControl.o ./ObjectCode/DigitizerStateMachine.o ./ObjectCode/DigitizerObjectGeneric.o ./ObjectCode/RawData.o ./ObjectCode/DigitizerErrorObject.o ./ObjectCode/Analizzatore.o ./ObjectCode/OutputModule.o ./ObjectCode/AnalizzatoreUtils.o ./ObjectCode/DigitizerObject.o ./ObjectCode/ConfObject.o -lCAENDigitizer -lpthread -std=gnu++11 -pthread

client: ./ObjectCode/ClientApplication.o
	gcc ./ObjectCode/ClientApplication.o -o objectdumpclient -lpthread

decode: ./ObjectCode/Decode.o
	gcc ./ObjectCode/Decode.o -o ./RawData/decode -lCAENDigitizer

decode742: ./ObjectCode/Decode742.o
	g++ ./ObjectCode/Decode742.o -o ./RawData/decode742 -lCAENDigitizer `root-config --glibs`

remove:
	rm -f ./ObjectCode/*.o
	rm -f objectdump
	rm -f objectdumpclient
	rm -f ./RawData/decode
	rm -f ./RawData/decode742

flex:
	flex ./SourceCode/AnalizzatoreLessicale.flex
	mv lex.yy.c ./SourceCode/Analizzatore.c

./ObjectCode/OutputModule.o: ./SourceCode/OutputModule.cpp
	g++ -c ./SourceCode/OutputModule.cpp -o ./ObjectCode/OutputModule.o -std=gnu++11 -pthread
./ObjectCode/Main.o: ./SourceCode/Main.c
	g++ -c ./SourceCode/Main.c -o ./ObjectCode/Main.o -std=gnu++11 -pthread
./ObjectCode/LogFile.o: ./SourceCode/LogFile.cpp
	g++ -c ./SourceCode/LogFile.cpp -o ./ObjectCode/LogFile.o -std=gnu++11 -pthread
./ObjectCode/ConfigurationConsistence.o: ./SourceCode/ConfigurationConsistence.cpp
	g++ -c ./SourceCode/ConfigurationConsistence.cpp -o ./ObjectCode/ConfigurationConsistence.o -std=gnu++11 -pthread
./ObjectCode/X742DecodeRoutines.o: ./SourceCode/X742DecodeRoutines.c
	g++ -c ./SourceCode/X742DecodeRoutines.c -o ./ObjectCode/X742DecodeRoutines.o -std=gnu++11 -pthread
./ObjectCode/TcpUser.o: ./SourceCode/TcpUser.cpp
	g++ -c ./SourceCode/TcpUser.cpp -o ./ObjectCode/TcpUser.o -std=gnu++11 -pthread
./ObjectCode/CommunicationObject.o: ./SourceCode/CommunicationObject.cpp
	g++ -c ./SourceCode/CommunicationObject.cpp -o ./ObjectCode/CommunicationObject.o -std=c++11 -pthread
./ObjectCode/ApplicationSetup.o: ./SourceCode/ApplicationSetup.cpp
	g++ -c ./SourceCode/ApplicationSetup.cpp -o ./ObjectCode/ApplicationSetup.o -std=gnu++11 -pthread
./ObjectCode/Input.o: ./SourceCode/Input.cpp
	g++ -c ./SourceCode/Input.cpp -o ./ObjectCode/Input.o -std=gnu++11 -pthread
./ObjectCode/DigitizerFlowControl.o: ./SourceCode/DigitizerFlowControl.cpp
	g++ -c ./SourceCode/DigitizerFlowControl.cpp -o ./ObjectCode/DigitizerFlowControl.o -std=gnu++11 -pthread
./ObjectCode/DigitizerStateMachine.o: ./SourceCode/DigitizerStateMachine.cpp
	g++ -c ./SourceCode/DigitizerStateMachine.cpp -o ./ObjectCode/DigitizerStateMachine.o -std=gnu++11 -pthread
./ObjectCode/DigitizerObjectGeneric.o: ./SourceCode/DigitizerObjectGeneric.cpp
	g++ -c ./SourceCode/DigitizerObjectGeneric.cpp -o ./ObjectCode/DigitizerObjectGeneric.o -std=gnu++11 -pthread
./ObjectCode/RawData.o: ./SourceCode/RawData.cpp
	g++ -c ./SourceCode/RawData.cpp -o ./ObjectCode/RawData.o -std=gnu++11 -pthread
./ObjectCode/DigitizerErrorObject.o: ./SourceCode/DigitizerErrorObject.cpp
	g++ -c ./SourceCode/DigitizerErrorObject.cpp -o ./ObjectCode/DigitizerErrorObject.o -std=gnu++11 -pthread
./ObjectCode/Analizzatore.o: ./SourceCode/Analizzatore.c
	g++ -c ./SourceCode/Analizzatore.c -o ./ObjectCode/Analizzatore.o -std=gnu++11 -pthread
./ObjectCode/AnalizzatoreUtils.o: ./SourceCode/AnalizzatoreUtils.c
	g++ -c ./SourceCode/AnalizzatoreUtils.c -o ./ObjectCode/AnalizzatoreUtils.o -std=gnu++11 -pthread
./ObjectCode/DigitizerObject.o: ./SourceCode/DigitizerObject.cpp
	g++ -c ./SourceCode/DigitizerObject.cpp -o ./ObjectCode/DigitizerObject.o -std=gnu++11 -pthread
./ObjectCode/ConfObject.o: ./SourceCode/ConfObject.cpp
	g++ -c ./SourceCode/ConfObject.cpp -o ./ObjectCode/ConfObject.o -std=gnu++11 -pthread
./ObjectCode/ClientApplication.o: ./SourceCodeClient/ClientApplication.c
	gcc -c ./SourceCodeClient/ClientApplication.c -o ./ObjectCode/ClientApplication.o
./ObjectCode/Decode.o: ./RawData/OfflineReadout.cpp
	gcc -c -fpermissive ./RawData/OfflineReadout.cpp -o ./ObjectCode/Decode.o
./ObjectCode/Decode742.o: ./RawData/OfflineReadoutX742.c 
	g++  `root-config --cflags` -c -fpermissive ./RawData/OfflineReadoutX742.c -o ./ObjectCode/Decode742.o `root-config --glibs`
