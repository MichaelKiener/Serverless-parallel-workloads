package example;

import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.lambda.runtime.LambdaLogger;

import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import java.util.concurrent.atomic.AtomicInteger;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Map;
import java.util.Scanner;

import mvtpackage.*;
// Handler value: example.Handler
public class Handler implements RequestHandler<Map<String,String>, String>{
  Gson gson = new GsonBuilder().setPrettyPrinting().create();
  private final AtomicInteger count = new AtomicInteger(0);
  @Override
  public String handleRequest(Map<String,String> event, Context context)
  {
    LambdaLogger logger = context.getLogger();
    // log execution details
    // logger.log("ENVIRONMENT VARIABLES: " + gson.toJson(System.getenv()));
    // logger.log("CONTEXT: " + gson.toJson(context));
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

    // process event
    String res = "Did not run";
    MVT mvt = new MVT();
    try {
    res = mvt.runMVT(event.get("Input"), Integer.parseInt(event.get("Threads")));
    } catch (Exception e) {
      logger.log(e.getMessage());
    }
    res += "\n" + addInfo;
    return res;
  }
}