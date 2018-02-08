#include <iostream>
#include <unistd.h>

#include <restclient-cpp/restclient.h>

void wait_for_gdb_to_attach() {
  int is_waiting = 1;
  while (is_waiting) {
    sleep(1); // sleep for 1 second
  }
}

using namespace std;

int main(void) {
  wait_for_gdb_to_attach();
  auto response = RestClient::post("https://slack.com/api/api.test", "application/json", "{\"foo\":\"bar\"}");
  cout << "Content-Type: text/plain;charset=us-ascii\n\n";
  cout << "Slack replied: " << response.body << "\n";
  return 0;
}
