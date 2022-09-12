#include "fish_go.h"
#include "test.h"
#include "timer.h"

using namespace std;

int main() {  // argc argv ???
  try {
    CreateTablePhysicalData();
    Timer t;
    // Test();
    TestLight();

  } catch (const Fish_go::ExceptionFish_go& e) {
    cerr << e.what() << endl;
    return 1;
  } catch (const pqxx::sql_error& e) {
    std::cerr << "~~~CATCH pqxx::sql_error"
              << "Database error: " << e.what() << std::endl
              << "Query was: " << e.query() << std::endl;
    return 2;
  } catch (const std::exception& e) {
    cerr << "~~~CATCH std::exception " << e.what() << std::endl;
    return 3;
  } catch (...) {
    cerr << "~~~CATCH Unknown exception, exit" << endl;
    return 4;
  }

  cout << "\n---SUCCESS---\n" << endl;
  return 0;
}
