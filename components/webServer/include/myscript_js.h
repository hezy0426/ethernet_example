#ifndef MYSCRIPT_JS_H
#define MYSCRIPT_JS_H

const char *jsString = "function fetchData() {\n        var xhr = new XMLHttpRequest();\n        xhr.onreadystatechange = function () {\n          if (xhr.readyState === XMLHttpRequest.DONE) {\n            if (xhr.status === 200) {\n            var jsonData = JSON.parse(xhr.responseText); // Parse the JSON response\n                    var outputValue = jsonData.output; // Extract the \"output\" value from the JSON\n              document.getElementById(\"counter\").innerText = outputValue;\n            } else {\n              console.error(\"Error:\", xhr.status);\n            }\n          }\n        };\n        xhr.open(\"GET\", \"/data\", true);\n        xhr.send();\n      }\n        // Fetch data initially\n        fetchData();\n\n        // Periodically fetch data every 5 seconds (5000 milliseconds)\n        setInterval(fetchData, 5000);";


#endif // MYSCRIPT_JS_H
