// @(#)log.cc	1.2 99/08/27 SBHRS

#include    "rr.h"
#include    "globals.h"

LogQ *logQ = NULL;

void LogQ::log(char *tstamp, const char *message) {
	Message *m = new Message();
	sprintf(m->text, "%s: %s", tstamp+4, message, 75);
	m->text[75] = '\0';
	m->n = NULL;
	m->p = head;
	if (head) {head->n = m;}
	head = m;
	if (tail == NULL) {tail = m;}
	modified++;
	counter++;
}

void logmessage(const char *message)
{
	time_t  now;        // time structure
	ofstream logfile;   // file for logging events
	char *nowstring;

	if (logQ == NULL) {
		logQ = new LogQ();
	}

	now = time(NULL);     // get time
	nowstring = ctime(&now);
	nowstring[strlen(nowstring) - 1] = '\0';

	logQ->log(nowstring, message);

	logfile.open(LOGFILE, ios::app);
	if (!logfile) {
		cout << "Cannot open LOGFILE: \"" << LOGFILE << "\"\n";
		exit(0);
	}
	logfile << nowstring << ": " << message << "\n";
	logfile.flush();
	logfile.close();
}

