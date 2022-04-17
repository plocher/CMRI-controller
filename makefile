
#ident "@(#)Makefile	1.4 99/08/18 SBHRS"

CC= c:\bc5\bin\bcc
CCC= c:\bc5\bin\bcc

BCFLAGS=-g
BCCFLAGS=-g

HEADERS	=	usic.h port.h rr.h

TESTSOURCES	= \
		port.cpp		\
		usic.cpp		\
		cards.cpp
TESTOBJECTS	= \
		port.obj		\
		usic.obj		\
		cards.obj

IOTESTSOURCES	=\
		cards.cpp	\
		sbhrs.cpp	\
		outbuf.cpp

IOTESTOBJECTS	=\
		cards.obj		\
		sbhrs.obj		\
		outbuf.obj


SOURCES	=	\
		assigndr.cpp	\
		autoad.cpp	\
		cabdir.cpp	\
		keyboard.cpp	\
		outbuf.cpp	\
		panic.cpp	\
		cards.cpp	\
		port.cpp	\
		usic.cpp	\
		kbdio.cpp	\
		rropbfsc.cpp	\
		rrdisp.cpp	\
		rrdebug.cpp	\
		rrlog.cpp	\
		rrcards.cpp	\
		rroperat.cpp	\
		signal.cpp	\
		turnout.cpp	\
		sbhrs.cpp	\
		crossenv.cpp	\
		log.cpp		\
		util.cpp

UNIXOBJECTS	=		assigndr.obj	\
			autoad.obj	\
			cabdir.obj	\
			keyboard.obj	\
			outbuf.obj	\
			panic.obj	\
			cards.obj	\
			port.obj	\
			usic.obj	\
			kbdio.obj	\
			rropbfsc.obj	\
			rrdisp.obj	\
			rrdebug.obj	\
			rrlog.obj	\
			rrcards.obj	\
			signal.obj	\
			sbhrs.obj	\
			turnout.obj	\
			crossenv.obj	\
			log.obj		\
			util.obj

DOSOBJECTS = assigndr.obj autoad.obj cabdir.obj keyboard.obj outbuf.obj panic.obj cards.obj port.obj usic.obj kbdio.obj rropbfsc.obj rrdebug.obj rrlog.obj rrdisp.obj rrcards.obj signal.obj sbhrs.obj turnout.obj crossenv.obj log.obj util.obj

COMMANDS	=	rroperat.exe testchub.exe testwire.exe
# COMMANDS	= testchub.exe test.exe testpanl.exe

all:	$(COMMANDS)

clean:
	erase *.obj
	erase *.exe

rroperat.exe:	$(HEADERS) $(SOURCES) $(DOSOBJECTS) rroperat.obj 
	$(CCC) rroperat.obj  @&&|
	$(DOSOBJECTS)
|

testchub.exe:	$(HEADERS) $(TESTSOURCES) util.cpp $(TESTOBJECTS) util.obj testchub.obj
	$(CCC) testchub.obj $(TESTOBJECTS) util.obj

testwire.exe:	$(HEADERS) $(IOTESTSOURCES) $(IOTESTOBJECTS) testwire.obj
	$(CCC) testwire.obj $(IOTESTOBJECTS)


testpanl.exe:	$(HEADERS) $(SOURCES) $(TESTOBJECTS) sbhrs.obj testpanl.obj 
	$(CCC) testpanl.obj $(TESTOBJECTS) sbhrs.obj

test.exe:	$(HEADERS) $(TESTSOURCES) $(TESTOBJECTS) test.obj
	$(CCC) test.obj $(TESTOBJECTS)


