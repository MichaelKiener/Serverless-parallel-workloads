/*
 * Copyright 2020 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package functions;

// [START functions_helloworld_http]

import com.google.cloud.functions.HttpFunction;
import com.google.cloud.functions.HttpRequest;
import com.google.cloud.functions.HttpResponse;
import com.google.gson.Gson;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParseException;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Logger;

import mvtpackage.*;

public class MVT_java implements HttpFunction {
  private final AtomicInteger count = new AtomicInteger(0);

  private static final Logger logger = Logger.getLogger(MVT_java.class.getName());

  private static final Gson gson = new Gson();

  @Override
  public void service(HttpRequest request, HttpResponse response)
      throws IOException {
        count.getAndIncrement();
        String executionID = "";
        List<String> executionIDs = request.getHeaders().get("Function-Execution-Id");
        for (int i = 0; i < executionIDs.size(); i++) {
          executionID = executionIDs.get(i);
        }
        //Header.Get("Function-Execution-Id")
        String addInfo = "InvocationCount: " + count.get() + "\n";
        if(count.get() <= 1) {
        try {
          File myObj = new File("/proc/cpuinfo");
          Scanner myReader = new Scanner(myObj);
          while (myReader.hasNextLine()) {
            String data = myReader.nextLine();
            System.out.println(data);
            addInfo += data + "\n";
          }
          myReader.close();
        } catch (FileNotFoundException e) {
          System.out.println("File not found");
          e.printStackTrace();
        }
    }
    // Check URL parameters for "name" field
    // "world" is the default value
    // String name = request.getFirstQueryParameter("name").orElse("world");
    int threads = 1;
    String input = "S";
    // Parse JSON request and check for "name" field
    try {
      JsonElement requestParsed = gson.fromJson(request.getReader(), JsonElement.class);
      JsonObject requestJson = null;

      if (requestParsed != null && requestParsed.isJsonObject()) {
        requestJson = requestParsed.getAsJsonObject();
      }
      if (requestJson != null && requestJson.has("Threads") && requestJson.has("Input")) {
        input = requestJson.get("Input").getAsString();
        threads = Integer.parseInt(requestJson.get("Threads").getAsString());
      }
    } catch (JsonParseException e) {
      logger.severe("Error parsing JSON: " + e.getMessage());
    }

    MVT mvt = new MVT();
    String results = "Did not run";
    try {
      results = mvt.runMVT(input, threads);
    } catch (Exception e) {
      results = "Error";
    }
    results += "\n" + addInfo;
    results += "\n ExecutionID: " + executionID;

    var writer = new PrintWriter(response.getWriter());
    writer.printf("%s", results);
  }
}
// [END functions_helloworld_http]
