#include <cstdlib>
#include <iostream>

using namespace std;
/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <chrono>
using namespace sql;

void test_multiple_query(Connection* con) {
  //std::unique_ptr<>
  sql::Statement* stmt = con->createStatement();
	// create a statement. Prepared statements are faster and more secure
	// if there are params. In this case there are none.
  //std::unique_ptr< sql::ResultSet >
  stmt->executeQuery("CALL sp_getvalues()");
#if 0
  do {
    res.reset(stmt.getResultSet());
    while (res.next()) {
     cout << res.getInt(1) << '\n';
    }
  } while (stmt.getMoreResults());
  #endif
  stmt->getMoreResults();
  sql::ResultSet* res = stmt->getResultSet(); 
  cout << res->getInt(1) << '\n';
  delete res;
  res = stmt->getResultSet();
  cout << res->getInt(1) << '\t' << res->getInt(2) << '\n';
  delete res;
  res = stmt->getResultSet();
  cout << res->getDouble(1)<< '\n';
  delete res;
  delete stmt;
}

void test_multiple_query2(Connection* con) {
  std::unique_ptr< sql::PreparedStatement >  pstmt;
  std::unique_ptr< sql::ResultSet > res;

  pstmt.reset(con->prepareStatement("CALL sp_getvalues()"));
  res.reset(pstmt->executeQuery());
  if (res->next()) {
    cout << res->getInt(1) << '\n';
  }
  if (pstmt->getMoreResults()) {
    res.reset(pstmt->getResultSet());
    if (res->next()) {
      cout << res->getInt(1) << '\t' << res->getInt(2) << '\n';
    }
  }
  if (pstmt->getMoreResults()) {
    res.reset(pstmt->getResultSet());
    if (res->next()) {
      cout << res->getDouble(1) << '\n';
    }
  }
}

int main(int argc, char* argv[]) {
try {
	//Note: DO NOT EMBED YOUR PASSWORD IN YOUR CODE. THIS PART OF THE EXAMPLE IS STUPID!
	const char* url = argc > 1 ? argv[1] : "tcp://127.0.0.1:3306";
	const char* userid = argc > 2 ? argv[2] : "dov";
	const char* passwd = argc > 3 ? argv[3] : "test";
	Driver* driver = get_driver_instance();
	if (driver == nullptr) throw "Could not get driver";

	// connect to hardcoded local mysql server
	// note: not good practice to embed password in code! just for first demo
  sql::ConnectOptionsMap connection_properties;
  connection_properties["CLIENT_MULTI_RESULTS"]= "true";
  connection_properties["hostName"]=url;
    /* user comes from the unit testing framework */
  connection_properties["userName"]=userid;
  connection_properties["password"]=passwd;
  connection_properties["useTls"]= "true";
	Connection* con = driver->connect(connection_properties);
  con->setSchema("dov");
//  test_multiple_query(con);
  test_multiple_query2(con);
	/*
		clean up. This is not a good way to do it, because if you forget you leak the objects
		TODO: use smart pointer to automatically deallocate.
		This API does not seem to work with std::unique_ptr
	*/
	delete con;
} catch (sql::SQLException &e) {
//  cout << "# ERR: SQLException in " << __FILE__;
//  cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
  cout << e.what() << endl;
  cout << e.getSQLState() << endl;
  cout << e.getErrorCode() << endl;

//  cout << "# ERR: " << e.what();
//  cout << " (MySQL error code: " << e.getErrorCode();
//  cout << ", SQLState: " << e.getSQLState() << " )" << endl;
} catch (const char* msg) {
    cerr << msg << endl;
}

cout << endl;

return 0;
}
