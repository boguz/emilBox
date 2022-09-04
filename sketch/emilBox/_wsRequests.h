/*
* Respond correctly when the index.html is requested
* (we send back the "index_html_gz" file array
*/
void onIndexRequest(AsyncWebServerRequest *request) {
  const char* dataType = "text/html";
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  AsyncWebServerResponse *response = request->beginResponse_P(200, dataType, index_html_gz, index_html_gz_len);
  response->addHeader("Content-Encoding", "gzip");
  request->send(response);
}

/*
* Respond with 404 with request file does not exist
*/
void onPageNotFound(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}