#pragma
#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include "targetver.h"
#include "mysql_connection.h"

#include <vector>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>


using namespace std;
using namespace sql;

class DBI
{
private:
	Connection *con = NULL;
	Driver *driver;
	Statement *stmt;
	ResultSet *res;

public:

	//Creates the connection. The way this is setup allows simple connections to be opened by just doing object.function...
	DBI(string connection_string, string usrname, string pword, string database_schema)
	{
		try {
			string conStr, username, password, schema;

			conStr = connection_string;
			username = usrname;
			password = pword;
			schema = database_schema;

			driver = get_driver_instance();
			con = driver->connect(conStr, username, password);
			con->setSchema(schema);
			cout << "Connected." << endl;
		}
		catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	}


	void endConnection()
	{
		delete con;
	}

	/*----------------------------------------------------------- USER -------------------------------------------------------------*/
	int InsertUser(string fname, string lname, string uname, string password)
	{
		int r;
		try {
			string statement;
			statement = "INSERT INTO STACDB.Users (`FName`, `LName`, `UName`, `Password`) VALUES('" + fname + "', '" + lname + "', '" + uname + "', '" + password + "');";
			stmt = con->createStatement();
			stmt->execute(statement);
			r = 0;
			delete stmt;
			return r;
		}
		catch (sql::SQLException &e) {
			r = 1;
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return r;
		}
		return r;
	}
	void deleteUser(string userId, string statement)
	{
		try {
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			delete res;
			delete stmt;
		}
		catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}

	}
	string selectUser(string user, string statement)
	{
		string resultU, resultI, result, finalResults;

		try {
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				resultU = res->getString("FName");
				resultI = res->getString("UName");
				result = resultU + " " + resultI + "\n";
				finalResults += result;
			}
			delete res;
			delete stmt;
			return finalResults;
		}
		catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	}

	int CheckUser(string uname, string password)
	{
		string resultP, statement;
		int r;
		try {
			statement = "SELECT `UName`,`Password` FROM STACDB.Users WHERE `UName`='" + uname + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				resultP = res->getString("Password");
			}
			if (resultP == password) {
				r = 0;
			}
			else {
				r = 1;
			}
			delete res;
			delete stmt;
			return r;
		}
		catch (sql::SQLException &e) {
			r = 1;
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return r;
		}
	}
	/*----------------------------------------------------- NEW USER FUNCTIONS ------------------------------------------------------*/

	vector<string> UserSelectClass(string username)
	{
		vector<string> resultV;
		string statement;
		string result, classID, userID;
		string lb = ",";
		try {
			statement = "SELECT `ID` FROM STACDB.Users WHERE `UName` = '" + username + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				userID = res->getString("ID");
			}
			statement = "SELECT `ClassID` from STACDB.Enrollment where UserID ='" + userID + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				classID = res->getString("ClassID");
				result = classID + lb;
				resultV.insert(resultV.end(), result);
			}
			delete res;
			delete stmt;
			return resultV;
		}
		catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	}

	vector<string> SelectEnrollmentList(string crn)
	{
		vector<string> resultU;
		vector<string> resultV;
		string statement;
		string result, UserID, UserName;
		string lb = " ";
		int n = 0;
		try {
			statement = "SELECT `UserID` from STACDB.Enrollment where `ClassID` = '" + crn + "' ;";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				UserID = res->getString("UserID");
				resultU.insert(resultU.end(), UserID);
			}
			while (n != resultU.size()) {
				statement = "SELECT `UName` from STACDB.Users where `ID` = '" + resultU[n] + "' ;";
				stmt = con->createStatement();
				res = stmt->executeQuery(statement);
				while (res->next()) {
					UserName = res->getString("UName");
					result = UserName + lb;
					resultV.insert(resultV.end(), result);
				}
				n++;
			}
			delete res;
			delete stmt;
			return resultV;
		}
		catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	}

	int InsertUserIntoEnrollment(string crn, string username, string deviceID, string deviceFlag)
	{
		int r;
		string statement, userID;
		try {
			statement = "SELECT `ID` FROM STACDB.Users WHERE `UName` = '" + username + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				userID = res->getString("ID");
			}
			statement = "INSERT INTO STACDB.Enrollment (`UserID`, `ClassID`, `DeviceID`, `DeviceChangeFlag`) VALUES('" + userID + "', '" + crn + "', '" + deviceID + "', '" + deviceFlag + "');";
			stmt = con->createStatement();
			stmt->execute(statement);
			r = 0;
			delete stmt;
			return r;
		}
		catch (sql::SQLException &e) {
			r = 1;
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return r;
		}
		return r;
	}

	int RemoveUserFromEnrolledClass(string crn, string username)
	{
		int r;
		string statement, userID;
		try {
			statement = "SELECT `ID` FROM STACDB.Users WHERE `UName` = '" + username + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				userID = res->getString("ID");
			}
			statement = "DELETE FROM STACDB.Enrollment WHERE `UserID`='" + userID + "' and`ClassID`='" + crn + "';";
			stmt = con->createStatement();
			stmt->execute(statement);
			r = 0;
			delete stmt;
			return r;
		}
		catch (sql::SQLException &e) {
			r = 1;
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return r;
		}
		return r;
	}

	/*----------------------------------------------------------- ADMIN -------------------------------------------------------------*/
	int InsertAdmin(string fname, string lname, string uname, string password)
	{
		int r;
		try {
			string statement;
			statement = "INSERT INTO STACDB.Admins (`FName`, `LName`, `UName`, `Password`) VALUES('" + fname + "', '" + lname + "', '" + uname + "', '" + password + "');";
			stmt = con->createStatement();
			stmt->execute(statement);
			r = 0;
			delete stmt;
			return r;
		}
		catch (sql::SQLException &e) {
			r = 1;
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return r;
		}
		return r;
	}

	int CheckAdmin(string uname, string password)
	{
		string resultP;
		int r;
		try {
			string statement;
			statement = "SELECT `UName`,`Password` FROM STACDB.Admins WHERE `UName`='" + uname + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				resultP = res->getString("Password");
			}
			if (resultP == password) {
				r = 0;
			}
			else {
				r = 1;
			}
			delete res;
			delete stmt;
			return r;
		}
		catch (sql::SQLException &e) {
			r = 1;
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return r;
		}
	}

	/*----------------------------------------------------- NEW ADMIN FUNCTIONS ------------------------------------------------------*/

	vector<string> AdminSelectClass(string username)
	{
		vector<string> resultV;
		string statement;
		string result, classID, userID;
		string lb = ",";
		try {
			statement = "SELECT `ID` FROM STACDB.Admins WHERE `UName` = '" + username + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				userID = res->getString("ID");
			}
			statement = "SELECT `ClassID` from STACDB.Classes where AdminID ='" + userID + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				classID = res->getString("ClassID");
				result = classID + lb;
				resultV.insert(resultV.end(), result);
			}
			delete res;
			delete stmt;
			return resultV;
		}
		catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	}

	int RemoveClass(string crn, string username)
	{
		int r;
		string statement, userID;
		try {
			statement = "SELECT `ID` FROM STACDB.Admins WHERE `UName` = '" + username + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				userID = res->getString("ID");
			}
			statement = "DELETE FROM STACDB.Classes WHERE `AdminID`='" + userID + "' AND `ClassID`='" + crn + "';";
			stmt = con->createStatement();
			stmt->execute(statement);
			r = 0;
			delete stmt;
			return r;
		}
		catch (sql::SQLException &e) {
			r = 1;
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return r;
		}
		return r;
	}

	/*----------------------------------------------------------- Classes -------------------------------------------------------------*/

	vector<string> SelectClassID(string classname, string institution)
	{
		vector<string> resultV;
		string statement;
		string result, classID;
		string lb = ",";
		try {
			statement = "SELECT `ClassID` FROM STACDB.Classes WHERE `ClassName` LIKE '%" + classname + "%' AND Institution LIKE '%" + institution + "%' ;";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				classID = res->getString("ClassID");
				result = classID + lb;
				resultV.insert(resultV.end(), result);
			}
			delete res;
			delete stmt;
			return resultV;
		}
		catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	}

	vector<string> SelectClassDetails(string classid)
	{
		vector<string> resultV;
		string statement;
		string result, classID, className, adminName, institution, meettime, startdate, enddate, publicip;
		try {
			statement = "SELECT ClassID, ClassName, Institution, MeetTimes, StartDate, EndDate, PublicIPAddress, UName FROM STACDB.Classes LEFT JOIN STACDB.Admins on STACDB.Classes.AdminID = STACDB.Admins.ID WHERE ClassID = '" + classid + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				classID = res->getString("ClassID");
				className = res->getString("ClassName");
				institution = res->getString("Institution");
				meettime = res->getString("MeetTimes");
				startdate = res->getString("StartDate");
				enddate = res->getString("EndDate");
				publicip = res->getString("PublicIPAddress");
				adminName = res->getString("UName");
				result = classID + " " + className + " " + adminName + " " + institution + " " + meettime + " " + startdate + " " + enddate + " " + publicip;
				resultV.insert(resultV.end(), result);
			}
			delete res;
			delete stmt;
			return resultV;
		}
		catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	}

	int createClass(string className, string adminID, string institution, string meettime, string startdate, string enddate, string publicip)
	{
		int r, random, uniqueNum = 1, returnID;
		string uniqueID, statement, classID, resultID, randomSTR;
		try {
			while (uniqueNum = 1)
			{

				randomSTR = "";
				for (int i = 0; i < 6; ++i)
				{
					random = rand() % 10;
					randomSTR = randomSTR + to_string(random);

				}
				classID = "2016" + randomSTR;

				//fake dates
				startdate = "2016-02-02";
				enddate = "2016-03-03";

				uniqueID = "SELECT ClassID FROM STACDB.Classes WHERE ClassID='" + classID + "' ;";
				stmt = con->createStatement();
				res = stmt->executeQuery(uniqueID);
				while (res->next()) {
					resultID = res->getString("ClassID");
				}
				if (resultID != classID) {
					statement = "INSERT INTO STACDB.Classes (`ClassID`, `ClassName`, `AdminID`, `Institution`, `MeetTimes`, `StartDate`, `EndDate`, `PublicIPAddress`) VALUES('" + classID + "', '" + className + "', '" + adminID + "', '" + institution + "', '" + meettime + "', '" + startdate + "', '" + enddate + "', '" + publicip + "');";
					stmt = con->createStatement();
					stmt->execute(statement);
					delete res;
					delete stmt;
					uniqueNum = 0;
					returnID = stoi(classID);
					return returnID;
				}
			}
			delete res;
			delete stmt;
		}
		catch (sql::SQLException &e) {
			r = 1;
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return r;
		}
	}

	/*------------------------------------------------------- ATTENDANCE ------------------------------------------------------------*/
	int InsertUserIntoAttendance(string crn, string username, string attnDate, string attnTime)
	{
		int r;
		string statement, UsersUserID, EnrollmentUserID, token, month, day, year;
		month = attnDate.substr(0, 2);
		day = attnDate.substr(3, 2);
		year = attnDate.substr(6, 4);
		string timestamp = year + "-" + month + "-" + day + " " + attnTime;
		try {
			statement = "SELECT `ID` FROM STACDB.Users WHERE `UName` = '" + username + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				UsersUserID = res->getString("ID");
			}
			statement = "SELECT `UserID` FROM STACDB.Enrollment WHERE `ClassID` = '" + crn + "' AND `UserID` = '" + UsersUserID + "' ;";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				EnrollmentUserID = res->getString("UserID");
			}
			if (UsersUserID == EnrollmentUserID) {
				statement = "INSERT INTO STACDB.Attendance (`UserID`, `ClassID`, `AttnTime`) VALUES('" + UsersUserID + "', '" + crn + "', '" + timestamp + "');";
				stmt = con->createStatement();
				stmt->execute(statement);
				r = 0;
				delete stmt;
			}
			else if (UsersUserID != EnrollmentUserID) {
				cout << "Cannot attend a class not enrolled in." << endl;
				r = 1;
			}
			return r;
		}
		catch (sql::SQLException &e) {
			r = 1;
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return r;
		}
		return r;
	}

	vector<string> SelectAttendanceDetailsUser(string classid, string username)
	{
		vector<string> resultV;
		string statement;
		string result, attnTime, attnYear, attnMonth, attnDay, attnDate, userID;
		string lb = " ";
		try {
			statement = "SELECT `ID` FROM STACDB.Users WHERE `UName` = '" + username + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				userID = res->getString("ID");
			}
			statement = "SELECT `AttnTime` FROM STACDB.Attendance WHERE `ClassID` = '" + classid + "' AND `UserID` = '" + userID + "' ;";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				attnTime = res->getString("AttnTime");

				attnYear = attnTime.substr(0, 4);
				attnMonth = attnTime.substr(5, 2);
				attnDay = attnTime.substr(8, 2);
				attnDate = attnMonth + "-" + attnDay + "-" + attnYear;
				result = attnDate + lb;
				resultV.insert(resultV.end(), result);
			}
			if (attnTime == "") {
				result = "00-00-0000";
				resultV.insert(resultV.end(), result);
				delete res;
				delete stmt;
				return resultV;
			}
			else {
				delete res;
				delete stmt;
				return resultV;
			}
		}
		catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	}

	int ManualInsertUserIntoAttendance(string crn, string username, string attnDate, string attnTime)
	{
		int r;
		string statement, month, day, year, UsersUserID, EnrollmentUserID;
		month = attnDate.substr(0, 2);
		day = attnDate.substr(3, 2);
		year = attnDate.substr(6, 4);
		string timestamp = year + "-" + month + "-" + day + " " + attnTime;
		try {
			statement = "SELECT `ID` FROM STACDB.Users WHERE `UName` = '" + username + "';";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				UsersUserID = res->getString("ID");
			}
			statement = "SELECT `UserID` FROM STACDB.Enrollment WHERE `ClassID` = '" + crn + "' AND `UserID` = '" + UsersUserID + "' ;";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				EnrollmentUserID = res->getString("UserID");
			}
			if (UsersUserID == EnrollmentUserID) {
				statement = "INSERT INTO STACDB.Attendance (`UserID`, `ClassID`, `AttnTime`) VALUES('" + UsersUserID + "', '" + crn + "', '" + timestamp + "');";
				stmt = con->createStatement();
				stmt->execute(statement);
				r = 0;
				delete stmt;
			}
			else if (UsersUserID != EnrollmentUserID) {
				cout << "Cannot attend a class not enrolled in." << endl;
				r = 1;
			}
			return r;
		}
		catch (sql::SQLException &e) {
			r = 1;
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
			return r;
		}
		return r;
	}

	vector<string> SelectAttendanceDetailsAdmin(string classid)
	{
		vector<string> resultV;
		vector<string> resultU;
		string statement;
		int n = 0;
		string result, count, userID, resU, username;
		string lb = " ";
		try {
			statement = "SELECT UserID FROM STACDB.Enrollment WHERE `ClassID` = '" + classid + "' ;";
			stmt = con->createStatement();
			res = stmt->executeQuery(statement);
			while (res->next()) {
				userID = res->getString("UserID");
				resultU.insert(resultU.end(), userID);
			}
			while (n != resultU.size()) {
				statement = "SELECT `UName` FROM STACDB.Users WHERE `ID` = '" + resultU[n] + "' ;";
				stmt = con->createStatement();
				res = stmt->executeQuery(statement);
				while (res->next()) {
					username = res->getString("UName");
				}
				statement = "SELECT COUNT(UserID) FROM STACDB.Attendance where `ClassID` = '" + classid + "' AND `UserID` = '" + resultU[n] + "' ;";
				stmt = con->createStatement();
				res = stmt->executeQuery(statement);
				while (res->next()) {
					count = res->getString("COUNT(UserID)");
					result = username + ";" + count + lb;
					resultV.insert(resultV.end(), result);
				}
				n++;
			}
			if (userID == "") {
				result = "F;0";
				resultV.insert(resultV.end(), result);
				delete res;
				delete stmt;
				return resultV;
			}
			else {
				delete res;
				delete stmt;
				return resultV;
			}
		}
		catch (sql::SQLException &e) {
			cout << "# ERR: SQLException in " << __FILE__;
			cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
			cout << "# ERR: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
	}



};



int main(void)
{
	string uname, password, userId, user, result, fname, lname, timestamp, deviceID, deviceFlag;
	char ans, ans2;

	deviceID = "FF:FF:FF:FF:FF:FF";
	deviceFlag = "0";

	string statement;
	int d;
	string dcstr, duname, dpword, dschema;
	string classDetails;
	string classname, institution, classid, adminID, meettime, startdate, enddate, publicip;
	string attnDate, attnTime;

	vector<string> v1;


	int n = 0;

	cout << "------------------- Connection Settings ------------------" << endl;
	cout << "1. Connect to Local Database" << endl;
	cout << "2. Connect to Class Database" << endl;
	cout << "3. Connect to a different Database" << endl;
	cout << "Enter: ";
	cin >> ans2;

	switch (ans2)
	{
	case '1':
		cout << "Connecting to local database..." << endl;
		dcstr = "tcp://127.0.0.1:3306";
		duname = "root";
		dpword = "wallacem12";
		dschema = "STACDB";
		break;
	case '2':
		cout << "Connecting to class database..." << endl;
		dcstr = "138.86.104.164:3306";
		duname = "SEClass";
		dpword = "BearsRock";
		dschema = "STACDB";
		break;
	case'3':
		cout << "Enter database connection string: ";
		cin >> dcstr;
		cout << "Enter database username: ";
		cin >> duname;
		cout << "Enter database password: ";
		cin >> dpword;
		cout << "Enter database schema: ";
		cin >> dschema;
		break;
	}

	//Make connection to the database
	DBI a(dcstr, duname, dpword, dschema);

	do {
		cout << "----------------------- Menu -----------------------" << endl;
		cout << "Enter the corresponding number to navigate the menu" << endl;
		cout << "a. Register (User)" << endl;
		cout << "b. Delete user" << endl;
		cout << "c. Retrieve data" << endl;
		cout << "d. Login (User)" << endl;
		cout << "e. Register (Admin)" << endl;
		cout << "f. Login (Admin)" << endl;
		cout << "g. End Connection to DB" << endl;
		cout << "h. Search Class" << endl;
		cout << "i. Class Details" << endl;
		cout << "j. Create Class" << endl;
		cout << "k. Exit" << endl;
		cout << "l. View User Class List" << endl;
		cout << "m. Enroll In Class" << endl;
		cout << "n. Drop User From Class" << endl;
		cout << "o. View Classes Created by Admin" << endl;
		cout << "p. Delete Class" << endl;
		cout << "q. Attend Class" << endl;
		cout << "r. Check Attendance (User)" << endl;
		cout << "s. Manually Attend Class (Admin)" << endl;
		cout << "t. Check Attendance Report (Admin)" << endl;
		cout << "u. View Class User List" << endl;
		cout << "Enter: ";
		cin >> ans;


		switch (ans)
		{
		case 'a':
			cout << "Enter in your first name: ";
			cin >> fname;
			cout << "Enter in your last name: ";
			cin >> lname;
			cout << "Enter in a username: ";
			cin >> uname;
			cout << "Enter in a password: ";
			cin >> password;
			d = a.InsertUser(fname, lname, uname, password);
			if (d == 0)
			{
				cout << endl << "Registered." << endl;
				system("pause");
			}
			else
			{
				cout << endl << "Register failed." << endl;
				system("pause");
			}
			break;
		case 'b':
			cout << "Enter in a userId: ";
			cin >> userId;
			statement = "DELETE FROM STACDB.Users WHERE `UName`='" + userId + "';";
			a.deleteUser(userId, statement);
			break;
		case 'c':
			cout << "Enter in the user you want to look up: ";
			cin >> user;
			//statement = "SELECT `username`,`ip` FROM test.user where `username` = '"+user+"';"; //Select statement for local database
			statement = "SELECT `FName`,`UName` FROM STACDB.Users where `FName` = '" + user + "';"; //Select statement for class database
			result = a.selectUser(user, statement);
			cout << "\nRetrieving data...." << endl;
			cout << "Result(s) from database: " << endl;
			cout << result;
			system("pause");
			break;
		case 'd':
			cout << "Enter in your username: ";
			cin >> uname;
			cout << "Enter in your password: ";
			cin >> password;
			d = a.CheckUser(uname, password);
			if (d == 0)
			{
				cout << "Logged in as " << uname << endl;
			}
			else
			{
				cout << "Failed to login." << endl;
			}
			system("pause");
			break;
		case 'e':
			cout << "Enter in your first name: ";
			cin >> fname;
			cout << "Enter in your last name: ";
			cin >> lname;
			cout << "Enter in a username: ";
			cin >> uname;
			cout << "Enter in a password: ";
			cin >> password;
			d = a.InsertAdmin(fname, lname, uname, password);
			if (d == 0)
			{
				cout << endl << "Registered." << endl;
				system("pause");
			}
			else
			{
				cout << endl << "Register failed." << endl;
				system("pause");
			}
			break;
		case 'f':
			cout << "Enter in your username: ";
			cin >> uname;
			cout << "Enter in your password: ";
			cin >> password;
			d = a.CheckAdmin(uname, password);
			if (d == 0)
			{
				cout << "Logged in as " << uname << endl;
			}
			else
			{
				cout << "Failed to login." << endl;
			}
			system("pause");
			break;
		case 'g':
			a.endConnection();
			break;
		case 'h':
			n = 0;
			cout << "Enter in the Class Name: ";
			cin >> classname;
			cout << "Enter in the Institution: ";
			cin >> institution;
			//classname = "";
			//institution = "";
			v1 = a.SelectClassID(classname, institution);
			while (n != v1.size()) {
				cout << v1[n];
				n++;
			}
			cout << endl;
			break;
		case 'i':
			cout << "Enter class id: ";
			cin >> classid;
			v1 = a.SelectClassDetails(classid);
			cout << v1[0] << endl;
			break;
		case 'j':
			cout << "Enter class name: ";
			cin >> classname;
			cout << "Enter admin ID: ";
			cin >> adminID;
			cout << "Enter institution: ";
			cin >> institution;
			cout << "Enter meettime: ";
			cin >> meettime;
			cout << "Enter start date (yyyy-mm-dd): ";
			cin >> startdate;
			cout << "Enter end date (yyyy-mm-dd): ";
			cin >> enddate;
			cout << "Enter public ip: ";
			cin >> publicip;
			d = a.createClass(classname, adminID, institution, meettime, startdate, enddate, publicip);
			if (d == 1)
			{
				cout << "Error inserting class." << endl;
				system("pause");
			}
			else
			{
				cout << "Class has been created with a class ID of " << d << endl;
				system("pause");
			}
			break;
		case 'l':
			n = 0;
			cout << "Enter the username: ";
			cin >> uname;
			v1 = a.UserSelectClass(uname);
			while (n != v1.size()) {
				cout << v1[n];
				n++;
			}
			cout << endl;
			break;
		case 'm':
			cout << "Enter class id: ";
			cin >> classid;
			cout << "Enter username: ";
			cin >> uname;
			d = a.InsertUserIntoEnrollment(classid, uname, deviceID, deviceFlag);
			if (d == 0) {
				cout << "Enrolled." << endl;
			}
			else {
				cout << "Enrollment failed." << endl;
			}
			break;
		case 'n':
			cout << "Enter class id: ";
			cin >> classid;
			cout << "Enter username: ";
			cin >> uname;
			d = a.RemoveUserFromEnrolledClass(classid, uname);
			if (d == 0) {
				cout << "Class Dropped." << endl;
			}
			else {
				cout << "Failed to Drop Class." << endl;
			}
			break;
		case 'o':
			n = 0;
			cout << "Enter the username: ";
			cin >> uname;
			v1 = a.AdminSelectClass(uname);
			while (n != v1.size()) {
				cout << v1[n];
				n++;
			}
			cout << endl;
			break;
		case 'p':
			cout << "Enter class id: ";
			cin >> classid;
			cout << "Enter username: ";
			cin >> uname;
			d = a.RemoveClass(classid, uname);
			if (d == 0) {
				cout << "Class Deleted." << endl;
			}
			else {
				cout << "Failed to Delete Class." << endl;
			}
			break;
		case 'q':
			cout << "Enter class id: ";
			cin >> classid;
			cout << "Enter username: ";
			cin >> uname;
			cout << "Enter date: ";
			cin >> attnDate;
			cout << "Enter time: ";
			cin >> attnTime;
			d = a.InsertUserIntoAttendance(classid, uname, attnDate, attnTime);
			if (d == 0) {
				cout << "Attendance Recorded." << endl;
			}
			else {
				cout << "Failed to Record Attendance." << endl;
			}
			break;
		case 'r':
			n = 0;
			cout << "Enter in the Class ID: ";
			cin >> classid;
			cout << "Enter in the username: ";
			cin >> uname;
			v1 = a.SelectAttendanceDetailsUser(classid, uname);
			while (n != v1.size()) {
				cout << v1[n];
				n++;
			}
			cout << endl;
			break;
		case 's':
			cout << "Enter class id: ";
			cin >> classid;
			cout << "Enter the user's username: ";
			cin >> uname;
			cout << "Enter date: ";
			cin >> attnDate;
			cout << "Enter time: ";
			cin >> attnTime;
			d = a.ManualInsertUserIntoAttendance(classid, uname, attnDate, attnTime);
			if (d == 0) {
				cout << "Attendance Recorded." << endl;
			}
			else {
				cout << "Failed to Record Attendance." << endl;
			}
			break;
		case 't':
			n = 0;
			cout << "Enter in the Class ID: ";
			cin >> classid;
			v1 = a.SelectAttendanceDetailsAdmin(classid);
			while (n != v1.size()) {
				cout << v1[n];
				n++;
			}
			cout << endl;
			break;
		case 'u':
			n = 0;
			cout << "Enter in the Class ID: ";
			cin >> classid;
			v1 = a.SelectEnrollmentList(classid);
			while (n != v1.size()) {
				cout << v1[n];
				n++;
			}
			cout << endl;
			break;
		case 'k':
			exit;
		}
	} while (ans != 'k');

	return 0;
}