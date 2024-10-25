//Programming Language: C++20
//IDE: Visual Studio
//Compile And Build In Console Using g++: g++ main.cpp -o programA
//Run In Console Using g++: Linux/Mac: ./programA       Windows: programA.exe
//OR Run the .exe application in the folder for program A
//Author: Connor Gilmore
//Date: 10/19/2024
//Class: CS 4500 - Intro To Software Profession
//Desc: This program automatically checks all Log CSV files in the folder. 
// Checks to make sure csv file format is correct,header details are correct, and log details are correct.
//Sources
//Code Reused And Adapted From Phase 1 Program B and C
////https://www.geeksforgeeks.org/cpp-program-to-get-the-list-of-files-in-a-directory/ get all files in my folder
// https://stackoverflow.com/questions/17095639/bind-a-char-to-an-enum-type set enum equal to characters
// https://www.geeksforgeeks.org/csv-file-management-using-c/ access/read/write for csv files 
// https://en.cppreference.com/w/cpp/utility/format/format format method I use to format datetime strings
//https://stackoverflow.com/questions/11213326/how-to-convert-a-string-variable-containing-time-to-time-t-type-in-c converting string to time_T object for comparison

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
//enum data structure to represent Activity codes clearly
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
//log entity data structure to represent a row in the csv file
struct LogDetails {
	string Date;
	string StartTime;
	string EndTime;
	string GroupSize;
	string ActivityCode;
	string Note;
	//Action: constructor to populate Log entity properties
  // Parameters: A vector of cells representing a single row in the CSV file. 
  // Each row in the CSV file corresponds to a single log entry.
	LogDetails(vector<string> cells)
	{
		const int MIN_CELLS = 5;
		const int NOTE_INCLUDED = 6;
		
		if (cells.size() < MIN_CELLS)
		{
			throw runtime_error("Not Enough Data Provided To Create Log Entity");
		}
		
	
		const int DATE_ROW = 0;
		const int START_TIME_ROW = 1;
		const int END_TIME_ROW = 2;
		const int GROUP_ROW = 3;
		const int CODE_ROW = 4;
		const int NOTE_ROW = 5;

	  Date = cells[DATE_ROW];
		StartTime = cells[START_TIME_ROW];
		EndTime = cells[END_TIME_ROW];
		GroupSize = cells[GROUP_ROW];
		ActivityCode = cells[CODE_ROW];
		if (cells.size() == NOTE_INCLUDED)
		{
			Note = cells[NOTE_ROW];
		}
	}
};

//Action: Prints Intro Screen
void WriteAppIntro();
//Action: checks if a character is a digit (0-9)
//Parameter: character 
//Return: true if char is between 0 and 9 (digit)
bool isCharacterADigit(char character);
//Action: Finds user's activity log file
//Returns: string that is the activity log file name
vector<string> FindActivityLogFiles();
//Action: get all the csv files in the folder 
//Return: a vector list of all the csv files in the folder 
vector<string> FetchAllCsvFileNamesInFolder();
//Action: looks at all the csv files in the folder for files that match are desired 'LastnameFirstnameLog.csv' format
//Parameter: fileNames (vector<string>), list of all csv file names in the folder
//Return: a vector list of all the csv files that match the format 'LastnameFirstnameLog.csv'
vector<string> FilterFilesForActivityLogFormat(vector<string> fileNames);
//Action: return error message if log file is empty.
// parse through all the cells in the cell csv file until an error is found, or all cells have been checked.
// if error is found stop parsing and report error.
// if all cells are valid return empty.
//Parameter:vector string representing csv log file name for access
//Return: error message or empty
string ValidateFile(string fileName);
//Action: error message if cells vector has more or less than two elements.
// error message if first name cell is not Alphabetical
// // error message if second name cell is not Alphabetical
// return empty string if full name row valid
//Parameter:vector representing cells in a csv row
//Return: error message or empty
string ValidateUsernameRow(vector<string> cells);
//Action: error message if cells vector has more or less than one element.
// error message if first and only elment(class name) does NOT equal 'CS 4500'
// return empty string if class row valid
//Parameter:vector representing cells in a csv row
//Return: error message or empty
string ValidateClassRow(vector<string> cells);
//Action: error message if time not in mm/dd/yyyy format. 
// if it is in correct format, than return empty string
//Parameter:string date text from csv file, int row number
//Return: error message or empty
string ValidateDate(string date, int rowCnt);
//Action: error message if time not in HH:MM format. 
// if it is in correct format, than return empty string
//Parameter:string time text from csv file, int row number
//Return: error message or empty
string ValidateTime(string time, int rowCnt);
//Action:  validate date and time format for date and time parameters. if any invalid then return error message
// return error message if we suspect user traveled back in time or worked more than 24 hours
// return warning message if user spent 4 or more hours on a activity
// return empty string if all is good
//Parameter: string date text, start time text, and end time text from csv file, int row number
//Return: error message or warning message or empty 
string ValidateTimeSpan(string date, string endTime, string startTime, int rowCnt);
//Action: if activity code is None (Unknown) return error message, else return empty string
//Parameter: string code text from csv file, int row number
//Return: error message or empty 
string ValidateGroup(string groupVal, int rowCnt);
//Action: if activity code is None (Unknown) return error message, else return empty string
//Parameter: string code text from csv file, int row number
//Return: error message or empty 
string ValidateActivityCode(string code, int rowCnt);
//Action: converts string from activity cell to a Activity enum
//Parameter: string code text from csv file
//Return: Activity enum
Activity StrToCode(string codeStr);
//Action: if activity code is other and note is empty then return error. 
// if note is more than 80 characters then return error
// if note has commas then return error
// else return empty string
//Parameter: string note text from csv file, activity enum code, int row number 
//Return: error message or empty string
string ValidateNote(string note, Activity code, int rowCnt);
//Action: return error message if log row is less than 5 cells or more than 6 cells. That would be invalid format
// if log row format good, build a log entity using data in that row and then validate that data.
//Parameter:vector representing cells in a csv row. int row number
//Return: error message or empty
string ParseLog(vector<string> cells, int rowCnt);
//Action: print ValidityChecks report to console
//Parameter: string report of problems found in csv log files
void GenerateConsoleReport(string report);
//Action: print ValidityChecks report to console
//Parameter: string report of problems found in csv log files
void GenerateFileReport(string report);
//Action: Convert string to dateTime object
//Parameter:datetime (string) in format mm/dd/yyy HH:MM
//Return:time_point representing an moment of time
system_clock::time_point StringToChronoDateTime(string datetime);
//Action: Prints Outro Screen
void WriteAppOutro();

string extraContent = "";

//Program A starts here
int main()
{
	try {
	string validityReport = "";
	
	WriteAppIntro();

	vector<string> activityLogs = FindActivityLogFiles();

	for (string& activityLog : activityLogs) {
		validityReport += "\n\n\n\nNow Validating Log File '" + activityLog + "':\n\n";
		validityReport += ValidateFile(activityLog);
		validityReport += extraContent;
		extraContent = "";
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
//Action: print ValidityChecks report to ValidityChecks text file
//Parameter: string report of problems found in csv log files
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
//Action: print ValidityChecks report to console
//Parameter: string report of problems found in csv log files
void GenerateConsoleReport(string report)
{
	cout << report << endl;
}
//Action: return error message if log file is empty.
// parse through all the cells in the cell csv file until an error is found, or all cells have been checked.
// if error is found stop parsing and report error.
// if all cells are valid return empty.
//Parameter:vector string representing csv log file name for access
//Return: error message or empty
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

		if (fileReport != "")
		{
			break;
		}

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
			fileReport = ValidateUsernameRow(cells);
		}
		else if (rowCounter == SECOND_ROW)
		{
			fileReport = ValidateClassRow(cells);
		}
		else
		{
			fileReport = ParseLog(cells, rowCounter);
		}
		rowCounter++;
	}

	if (rowCounter < REQUIRED_AMOUNT_OF_ROWS)
	{
		fileReport = "Log File Error: File Is Empty!\n";
	}

	file.close();

	return fileReport;

}

//Action: return error message if log row is less than 5 cells or more than 6 cells. That would be invalid format
// if log row format good, build a log entity using data in that row and then validate that data.
//Parameter:vector representing cells in a csv row. int row number
//Return: error message or empty
string ParseLog(vector<string> cells, int rowCnt)
{
	const int MIN_ROW_SIZE = 5;
	const int MAX_ROW_SIZE = 6;
	if (cells.size() < MIN_ROW_SIZE)
	{
		return "Line " + to_string(rowCnt + 1) + " Error: Missing " + to_string(MIN_ROW_SIZE - cells.size()) + " Cell(s).\n";
	}
	if (cells.size() > MAX_ROW_SIZE)
	{
		return "Line " + to_string(rowCnt + 1) + " Error: You Have " + to_string(cells.size() - MAX_ROW_SIZE) + " Extra Cell(s).\n";
	}
	
	string logReport = "";

	LogDetails log(cells);

	logReport = (logReport == "") ? ValidateTimeSpan(log.Date, log.StartTime, log.EndTime, rowCnt) : logReport;
	logReport = (logReport == "") ? ValidateGroup(log.GroupSize, rowCnt) : logReport;
	logReport = (logReport == "") ? ValidateActivityCode(log.ActivityCode, rowCnt) : logReport;
	logReport = (logReport == "") ? ValidateNote(log.Note, StrToCode(log.ActivityCode), rowCnt) : logReport;
	

	return logReport;
}
//Action: error message if cells vector has more or less than two elements.
// error message if first name cell is not Alphabetical
// // error message if second name cell is not Alphabetical
// return empty string if full name row valid
//Parameter:vector representing cells in a csv row
//Return: error message or empty
string ValidateUsernameRow(vector<string> cells)
{
	string nameReport = "";
	const int FIRST_ROW_SIZE = 2;
	if (cells.size() != FIRST_ROW_SIZE)
	{
		return " Line 1 Error: Row 1 Must Only Contain 2 Columns. Column 1 For 'LastName' And Column 2 For'FirstName'. Anything Else In Row 1 Is Invalid\n";

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
//Action: error message if cells vector has more or less than one element.
// error message if first and only elment(class name) does NOT equal 'CS 4500'
// return empty string if class row valid
//Parameter:vector representing cells in a csv row
//Return: error message or empty
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
//Action: error message if time not in mm/dd/yyyy format. 
// if it is in correct format, than return empty string
//Parameter:string date text from csv file, int row number
//Return: error message or empty
string ValidateDate(string date, int rowCnt)
{
	regex monthDayYearFormat("^(0[1-9]|1[0-2])/(0[1-9]|[12][0-9]|3[01])/(19|20)\\d{2}$");


	if (regex_match(date, monthDayYearFormat))
	{
		return "";
	}
	else
	{
		return "Line " + to_string(rowCnt + 1) + " Error: Invalid Date Format. Required Format: MM/DD/YYYY  \n";
	}
}
//Action: error message if time not in HH:MM format. 
// if it is in correct format, than return empty string
//Parameter:string time text from csv file, int row number
//Return: error message or empty
string ValidateTime(string time, int rowCnt)
{
	regex hourMinuteFormat("^([0-1][0-9]|2[0-3]):([0-5][0-9])$");

	if (regex_match(time, hourMinuteFormat)) {
		return "";
	}
	else {
		return "Line " + to_string(rowCnt + 1) + " Error: Invalid Time Format. Required Format: HH:MM \n";
	}
}
//Action: Convert string to dateTime object
//Parameter:datetime (string) in format mm/dd/yyy HH:MM
//Return:time_point representing an moment of time
system_clock::time_point StringToChronoDateTime(string datetime) 
{
		tm tm = {};
		istringstream ss(datetime);
		ss >> get_time(&tm, "%m/%d/%Y %H:%M");
		time_t time = mktime(&tm);
		return system_clock::from_time_t(time);
}
//Action:  validate date and time format for date and time parameters. if any invalid then return error message
// return error message if we suspect user traveled back in time or worked more than 24 hours
// return warning message if user spent 4 or more hours on a activity
// return empty string if all is good
//Parameter: string date text, start time text, and end time text from csv file, int row number
//Return: error message or warning message or empty 
string ValidateTimeSpan(string date, string startTime, string endTime, int rowCnt)
{
	string timeReport = "";

	timeReport = (timeReport == "") ? ValidateDate(date, rowCnt) : timeReport;
	timeReport = (timeReport == "") ? ValidateTime(startTime, rowCnt) : timeReport;
	timeReport = (timeReport == "") ? ValidateTime(endTime, rowCnt) : timeReport;

	if (timeReport != "")
	{
		return timeReport;
	}

	const int INVALID_TIME_DURATION = 0;
	const int FOUR_HOURS = 240;
	string sDateTime = date + " " + startTime;
	string eDateTime = date + " " + endTime;
	system_clock::time_point sTimePoint = StringToChronoDateTime(sDateTime);
	system_clock::time_point eTimePoint = StringToChronoDateTime(eDateTime);
	minutes timeSpent = duration_cast<minutes>(eTimePoint - sTimePoint);
	
	if (timeSpent.count() < INVALID_TIME_DURATION)
	{
		timeReport = "Line " + to_string(rowCnt + 1) + " Error: It Seems May Have Worked More Than 24 Hours On An Activity? \n";
	}

	if (timeSpent.count() >= FOUR_HOURS)
	{
		extraContent = "Line " + to_string(rowCnt + 1) + " Warning: Did You Really Spend Four Or More Hours On An Activity?\n";
	}

	return timeReport;
}
//Action: if group number is below 1 or above 50 then return error message, else return empty string
//Parameter: string group input text from csv file, int row number
//Return: error message or empty 
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
//Action: if activity code is None (Unknown) return error message, else return empty string
//Parameter: string code text from csv file, int row number
//Return: error message or empty 
string ValidateActivityCode(string code, int rowCnt)
{
	if (StrToCode(code) == Activity::None)
	{
		return "Line " + to_string(rowCnt + 1) + " Error: Activity Code Is Not Valid\n";
	}
	
	return "";
}
//Action: checks if a character is a digit (0-9)
//Parameter: character 
//Return: true if char is between 0 and 9 (digit)
bool isCharacterADigit(char character)
{
	int asciiValueForCharacter = static_cast<int>(character);

	const int ASCII_DIGIT_0 = 48;
	const int ASCII_DIGIT_9 = 57;
	if (asciiValueForCharacter < ASCII_DIGIT_0 || asciiValueForCharacter > ASCII_DIGIT_9)
	{
		return false;
	}
	else
	{
		return true;
	}
}
//Action: converts string from activity cell to a Activity enum
//Parameter: string code text from csv file
//Return: Activity enum
Activity StrToCode(string codeStr)
{
	const int MAX_CODE_SIZE = 1;
	if (codeStr.length() > MAX_CODE_SIZE)
	{
		return Activity::None;
	}
	char code = codeStr[0];
	if (isCharacterADigit(code) == false)
	{
		code = toupper(code);
	}
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
//Action: if activity code is other and note is empty then return error. 
// if note is more than 80 characters then return error
// if note has commas then return error
// else return empty string
//Parameter: string note text from csv file, activity enum code, int row number 
//Return: error message or empty string
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
//Action: Prints Intro Screen
void WriteAppIntro()
{
	cout << "\n\n\n\n";
	cout << "\nWelcome To Program A!\n" << endl;
	cout << "\nProgram A: Validity Checks on Multiple Log Files\n" << endl;
	cout << "\nAuthor: Connor Gilmore\n" << endl;
	cout << "\nGroup: Team 1\n" << endl;
	cout << "\nAbout: Verify format and content of Activity log files in the current folder.\n" << endl;
	cout << "\n\Summary: Press Enter and program automatically checks all Log CSV files in the folder. Checks to make sure csv file format is correct,header details are correct, and log details are correct." << endl;
	cout << "Header Issues: extra or too few cells, class is not called 'CS 4500' or missing, first and last name are not names" << endl;
	cout << "Log Issues:extra or too few cells, incorrect date and time format, start/end times span more than 4 hours(warning), start/end times span more than 24 hours, group size is not an whole number between 1 - 50 inclusivly, unknown activity code, empty note when activity code is 'Other', note is more than 80 characters, note has commas.\n" << endl;
	cout << "\n\n\n\n";
	cout << "Ready to log? Press Enter to continue\n" << endl;

	cin.ignore();
}
//Action: Prints Outro Screen
void WriteAppOutro()
{
	cout << "\n\nConclusion:\nAll Of The Log Files In This Folder Have Been Validated! Look At ValidityChecks.txt To See Your Errors And Warnings As Well!\n\n\n\n\n" << endl;
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
		throw runtime_error("File Error: No CSV files exist that match the format 'XLog.csv' in the folder. (X being 1 or more alphabetical characters.)");
	}

	return filteredCsvFileNames;
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
//FOR VISUAL STUDIO IDE ONLY:
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
