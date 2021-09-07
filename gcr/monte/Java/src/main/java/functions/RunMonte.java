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

import montepackage.*;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import java.util.concurrent.atomic.AtomicInteger;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Map;
import java.util.Scanner;
import java.util.*;

@SpringBootApplication
public class RunMonte {
  private final AtomicInteger count = new AtomicInteger(0);
  @Value("${NAME:World}")
  String name;

  @RestController
  class MonteController {
    @PostMapping(path= "/{id}", consumes = "application/json")
    String execute(@RequestBody Map<String, String> inputParams) throws NumberFormatException, InterruptedException  {
      count.getAndIncrement();
      String addInfo = "InvocationCount: " + count.get() + "\n";
      if(count.get() <= 1) {
      try {
        File myObj = new File("/proc/cpuinfo");
        Scanner myReader = new Scanner(myObj);
        while (myReader.hasNextLine()) {
          String data = myReader.nextLine();
          addInfo += data + "\n";
        }
        myReader.close();
      } catch (FileNotFoundException e) {
        System.out.println("File not found");
        e.printStackTrace();
      }
    }
      Monte monte = new Monte();
      String res = monte.runMonte(inputParams.get("Input"), Integer.parseInt(inputParams.get("Threads")));
      res += "\n" + addInfo;
      return res;
    }
  }

  public static void main(String[] args) {
    SpringApplication.run(RunMonte.class, args);
  }
}
// [END functions_helloworld_http]
