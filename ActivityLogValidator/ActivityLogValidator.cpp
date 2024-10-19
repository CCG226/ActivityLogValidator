// ActivityLogValidator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <format>
#include <regex>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iomanip>
#include <ctime>

using namespace std;
using namespace std::filesystem;
using namespace std::chrono;
enum class Activity {

	ReadTextbookOrCanvas,
	StudyPracticeQuiz,
	TakeScoringQuiz,
	CanvasDiscussion,
	TeamMeeting,
	DocumentationWork,
	WorkOnDesigns,
	Programming,
	ProgramTestingOrTestPlan,
	StudyForExam,
	ProfessorMeeting,
	MiniLectureTask,
	ReadOrWatchOutsideContent,
	Other,
	None

};
struct LogDetails {
	string Date;
	string StartTime;
	string EndTime;
	string GroupSize;
	string ActivityCode;
	string Note;
	LogDetails(vector<string> cells)
	{
		const int DATE_ROW = 2;
		const int START_TIME_ROW = 3;
		const int END_TIME_ROW = 4;
		const int GROUP_ROW = 5;
		const int CODE_ROW = 6;
		const int NOTE_ROW = 7;

	  Date = cells[DATE_ROW];
		StartTime = cells[START_TIME_ROW];
		EndTime = cells[END_TIME_ROW];
		GroupSize = cells[GROUP_ROW];
		ActivityCode = cells[CODE_ROW];
		Note = cells[NOTE_ROW];

	}
};
class LogException : public std::exception {
public:

	const char* what() const noexcept override {
		return "LOG ERROR";
	}
};
void WriteAppIntro();
vector<string> FindActivityLogFiles();
vector<string> FetchAllCsvFileNamesInFolder();
vector<string> FilterFilesForActivityLogFormat(vector<string> fileNames);
string ValidateFile(string fileName);
string ValidateUsernameRow(vector<string> cells);
string ValidateClassRow(vector<string> cells);
string ValidateDate(string date, int rowCnt);
string ValidateTime(string time, int rowCnt);
string ValidateTimeSpan(string date, string endTime, string startTime, int rowCnt);
string ValidateGroup(string groupVal, int rowCnt);
string ValidateActivityCode(string code, int rowCnt);
Activity StrToCode(string codeStr);
string ValidateNote(string note, Activity code, int rowCnt);
string ParseLog(vector<string> cells, int rowCnt);
void GenerateConsoleReport(string report);
void GenerateFileReport(string report);
system_clock::time_point StringToChronoDateTime(string datetime);
void ProblemReport(string msg);
void WriteAppOutro();
int main()
{
	try {
	string validityReport = "";
	
	WriteAppIntro();

	vector<string> activityLogs = FindActivityLogFiles();

	for (string& activityLog : activityLogs) {
		validityReport += "\n\n\n\nNow Validating Log File '" + activityLog + "':\n\n";
		validityReport += ValidateFile(activityLog);
		validityReport += "\n";
	}

	GenerateFileReport(validityReport);
	GenerateConsoleReport(validityReport);


	WriteAppOutro();
	}
	catch (const exception& e) {
		cerr << endl;
		cerr << e.what() << endl;
		cerr << endl;
		cerr << "Press Enter To Exit Program" << endl;
		cin.ignore();
		exit(1);
	}
}
void ProblemReport(string msg)
{
	throw LogException();
}
void GenerateFileReport(string report)
{
	const string OUTPUT_FILE = "ValidityChecks.txt";

	ofstream file(OUTPUT_FILE);
	
	if (file.is_open() == false) {
		throw runtime_error("File Error: Could not open the ValidityChecks Text file.");
	}

	file << report;

	file.close();
}
void GenerateConsoleReport(string report)
{
	cout << report << endl;
}
string ValidateFile(string fileName)
{
	string fileReport = "";
	ifstream file(fileName);

	if (file.is_open() == false) {
		throw runtime_error("File Error: Could not open the CSV file.");
	}
	const int FIRST_ROW = 0;
	const int SECOND_ROW = 1;
	const int REQUIRED_AMOUNT_OF_ROWS = 1;

	string row;

	int rowCounter = 0;

	while (getline(file, row)) {
		vector<string> cells;
		stringstream streamForRow(row);
		string cell;


		while (getline(streamForRow, cell, ',')) {
			if (cell != "")
			{
				cells.push_back(cell);
				cout << cell + "\n" << endl;
			}
		}
		cout << to_string(cells.size()) + "\n" << endl;
		if (rowCounter == FIRST_ROW)
		{
			return ValidateUsernameRow(cells);
		}
		if (rowCounter == SECOND_ROW)
		{
			return ValidateClassRow(cells);
		}
		else
		{
			return ParseLog(cells, rowCounter);
		}
		rowCounter++;
	}

	if (rowCounter <= REQUIRED_AMOUNT_OF_ROWS)
	{
		return "Log File Error: File Is Empty!\n";
	}

	file.close();

	return fileReport;

}
string ParseLog(vector<string> cells, int rowCnt)
{
	const int MIN_ROW_SIZE = 5;
	const int MAX_ROW_SIZE = 6;
	if (cells.size() < MIN_ROW_SIZE)
	{
		return "Line " + to_string(rowCnt + 1) + " Error: Missing " + to_string(MIN_ROW_SIZE - cells.size()) + " Cells.\n";
	}
	if (cells.size() < MIN_ROW_SIZE)
	{
		return "Line " + to_string(rowCnt + 1) + " Error: You Have " + to_string(cells.size() - MAX_ROW_SIZE) + " Extra Cells.\n";
	}
	
	string logReport = "";

	LogDetails log(cells);
	logReport = (logReport == "") ? ValidateTimeSpan(log.Date, log.StartTime, log.EndTime, rowCnt) : logReport;
	logReport = (logReport == "") ? ValidateGroup(log.GroupSize, rowCnt) : logReport;
	logReport = (logReport == "") ? ValidateActivityCode(log.ActivityCode, rowCnt) : logReport;
	logReport = (logReport == "") ? ValidateNote(log.Note, StrToCode(log.ActivityCode), rowCnt) : logReport;
	return logReport;
}

string ValidateUsernameRow(vector<string> cells)
{
	string nameReport = "";
	const int FIRST_ROW_SIZE = 2;
	if (cells.size() != FIRST_ROW_SIZE)
	{
		return " Line 1 Error: Row 1 Can Only Contain 2 Columns. Column 1 For 'LastName' And Column 2 For'FirstName'. Anything Else In Row 1 Is Invalid\n";

	}
	string fName = cells[0];
	string lName = cells[1];
	regex namePattern(R"(^[a-zA-Z]+$)");
	if (regex_match(fName, namePattern) == false)
	{
		return " Line 1 Error: First Name Is Invalid. Names Must Be Alphabetical Characters Only.\n";
	}
	if (regex_match(lName, namePattern) == false)
	{
		return " Line 1 Error: Last Name Is Invalid. Names Must Be Alphabetical Characters Only.\n";
	}
	return "";
}
string ValidateClassRow(vector<string> cells)
{
	
	const int SECOND_ROW_SIZE = 1;
	const string VALID_NAME = "CS 4500";
	if (cells.size() != SECOND_ROW_SIZE)
	{
		return " Line 2 Error: Row 2 Can Only Contain 1 Column. Column 1 For 'Class Name'. Anything Else In Row 2 Is Invalid\n";

	}
	string className = cells[0];
	if (className != VALID_NAME)
	{
		return " Line 2 Error: Class Name MUST Be 'CS 4500'. Anything Else Is Invalid.\n";
	}
	return "";
}
string ValidateDate(string date, int rowCnt)
{
	regex monthDayYearFormat("^(0[1-9]|1[0-2])/(0[1-9]|[12][0-9]|3[01])/(19|20)\\d{2}$");


	if (regex_match(date, monthDayYearFormat))
	{
		return "";
	}
	else
	{
		return "Line " + to_string(rowCnt + 1) + " Error: Invalid Date Format On Row \n";
	}
}
string ValidateTime(string time, int rowCnt)
{
	regex hourMinuteFormat("^([0-1][0-9]|2[0-3]):([0-5][0-9])$");

	if (regex_match(time, hourMinuteFormat)) {
		return "";
	}
	else {
		return "Line " + to_string(rowCnt + 1) + " Error: Invalid Time Format On Row \n";
	}
}

system_clock::time_point StringToChronoDateTime(string datetime) {
		tm tm = {};
		istringstream ss(datetime);
		ss >> get_time(&tm, "%m/%d/%Y %H:%M");
		time_t time = mktime(&tm);
		return system_clock::from_time_t(time);
	}

string ValidateTimeSpan(string date, string startTime, string endTime, int rowCnt)
{
	string timeReport = "";

	timeReport = (timeReport == "") ? ValidateDate(date, rowCnt) : timeReport;
	timeReport = (timeReport == "") ? ValidateTime(startTime, rowCnt) : timeReport;
	timeReport = (timeReport == "") ? ValidateTime(endTime, rowCnt) : timeReport;

	if (timeReport != "")
	{
		return "";
	}

	const int INVALID_TIME_DURATION = 0;
	const int MORE_THAN_FOUR_HOURS = 4;
	string sDateTime = date + " " + startTime;
	string eDateTime = date + " " + endTime;
	system_clock::time_point sTimePoint = StringToChronoDateTime(sDateTime);
	system_clock::time_point eTimePoint = StringToChronoDateTime(eDateTime);
	hours timeSpent = duration_cast<hours>(eTimePoint - sTimePoint);
	if (timeSpent.count() <= INVALID_TIME_DURATION)
	{
		return "Line " + to_string(rowCnt + 1) + " Error: It Seems May Have Worked More Than 24 Hours On An Activity? I Dont Believe So.\n";
	}

	if (timeSpent.count() >= MORE_THAN_FOUR_HOURS)
	{
		return "Line " + to_string(rowCnt + 1) + " Warning: Did You Really Spend Four Or More Hours On An Activity?\n";
	}

	return "";
}
string ValidateGroup(string groupVal, int rowCnt)
{
	const int MIN = 1;
	const int MAX = 50;
	try {
		int groupAmount = stoi(groupVal);
		if (groupAmount < MIN || groupAmount > MAX)
		{
			throw out_of_range("Group Size Has To Be Between 1 - 50");
		}
		
		return "";
	}
	catch (const invalid_argument& e) {
		return "Line " + to_string(rowCnt + 1) + " Error: Group Amount Must Be A Whole Positive Number\n";
	}
	catch (const out_of_range& e) {
		return "Line " + to_string(rowCnt + 1) + " Error: Group Amount Must Be A Whole Number Between 1 And 50\n";
	}

}
string ValidateActivityCode(string code, int rowCnt)
{
	if (StrToCode(code) == Activity::None)
	{
		return "Line " + to_string(rowCnt + 1) + " Error: Activity Code Is Not Valid\n";
	}
	
	return "";
}
Activity StrToCode(string codeStr)
{
	const int MAX_CODE_SIZE = 1;
	if (codeStr.length() > MAX_CODE_SIZE)
	{
		return Activity::None;
	}
	char code = codeStr[0];
	switch (code)
	{
	case '0':
		return Activity::ReadTextbookOrCanvas;
	case '1':
		return Activity::StudyPracticeQuiz;
	case '2':
		return Activity::TakeScoringQuiz;
	case '3':
		return Activity::CanvasDiscussion;
	case '4':
		return Activity::TeamMeeting;
	case '5':
		return Activity::DocumentationWork;
	case '6':
		return Activity::WorkOnDesigns;
	case '7':
		return Activity::Programming;
	case '8':
		return Activity::ProgramTestingOrTestPlan;
	case '9':
		return Activity::StudyForExam;
	case 'A':
		return Activity::ProfessorMeeting;
	case 'B':
		return Activity::MiniLectureTask;
	case 'C':
		return Activity::ReadOrWatchOutsideContent;
	case 'D':
		return Activity::Other;
	default: 
		return Activity::None;
	}
}
string ValidateNote(string note, Activity code, int rowCnt)
{
	
	const int MAX = 80;
	if (code == Activity::Other)
	{
		if (note == "")
		{
			return "Line " + to_string(rowCnt + 1) + " Error: Activity Is Other, BUT Note Is Empty\n";

		}
	}
	if (note.length() > MAX)
	{
		return "Line " + to_string(rowCnt + 1) + " Error: Note Is Longer Than 80 Characters!\n";
	}
	if (note.find(',') != string::npos) {
		return "Line " + to_string(rowCnt + 1) + " Error: Note Contains Commas!\n";
	}
	return "";
}
void WriteAppIntro()
{
	cout << "\n\n\n\n";
	cout << "\nWelcome To Program A!\n" << endl;
	cout << "\nProgram A: Validity Checks on Multiple Log Files\n" << endl;
	cout << "\nAuthor: Connor Gilmore\n" << endl;
	cout << "\nGroup: Team 1\n" << endl;
	cout << "\nDetails: Verify format and content of Activity log files in the current folder.\n" << endl;
	cout << "\n\Summary: Inputs from the user's keyboard and a user's csv file following the format 'LastnameFirstnameLog.csv'; outputs to the console and to the user's csv file. Application designed to be an easy way for user's to log activites. This program generates a new log to the csv file when the user is done working on their activity via console inputs!\n" << endl;
	cout << "\n\n\n\n";
	cout << "Ready to log? Press Enter to continue\n" << endl;

	cin.ignore();
}
void WriteAppOutro()
{
	cout << "\n\nAll Of The Log Files In This Folder Have Been Validated! Look At ValidityChecks.txt To See Your Errors And Warnings As Well!\n\n\n\n\n" << endl;
}
//Action: Finds user's activity log file
//Returns: string that is the activity log file name
vector<string> FindActivityLogFiles()
{
	vector<string> csvFileNames = FetchAllCsvFileNamesInFolder();
	if (csvFileNames.empty())
	{
		throw runtime_error("File Error: No CSV files exist in the current folder.");
	}

	vector<string> filteredCsvFileNames = FilterFilesForActivityLogFormat(csvFileNames);

	if (filteredCsvFileNames.empty())
	{
		throw runtime_error("File Error: No CSV files exist that match the format 'LastnameFirstnameLog.csv' in the folder.");
	}

	return csvFileNames;
}
//Action: get all the csv files in the folder 
//Return: a vector list of all the csv files in the folder 
vector<string> FetchAllCsvFileNamesInFolder()
{

	const string CSV_EXTENSION = ".csv";

	vector<string> results;

	path pathOfCurrentFolder = current_path();

	for (const auto& file : directory_iterator(pathOfCurrentFolder)) {
		if (file.path().extension() == CSV_EXTENSION) {

			string fileName = file.path().filename().string();

			results.push_back(fileName);
		}
	}

	return results;
}
//Action: looks at all the csv files in the folder for files that match are desired 'LastnameFirstnameLog.csv' format
//Parameter: fileNames (vector<string>), list of all csv file names in the folder
//Return: a vector list of all the csv files that match the format 'LastnameFirstnameLog.csv'
vector<string> FilterFilesForActivityLogFormat(vector<string> fileNames)
{
	vector<string> results;

	for (string& fileName : fileNames) {
		fileName = fileName.substr(0, fileName.size() - 4);//this is just removing the .csv ext
	}

	regex csvFileActivityLogPattern(R"(^[a-zA-Z]+Log$)");

	for (const string& fileName : fileNames) {
		if (regex_match(fileName, csvFileActivityLogPattern))
		{
			results.push_back(fileName + ".csv");
		}
	}

	return results;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
