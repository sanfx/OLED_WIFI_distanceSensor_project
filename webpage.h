namespace webpage
{

char html[] = "<html>\n\t\
  <head>\n\t\t\
    <title>Krysto San Nitin's Room</title>\n\t\t\
    <meta http-equiv='Content-Type' content='text/html;charset=utf-8' />\n\t\t\
    <meta name=\"apple-mobile-web-app-capable\" content=\"yes\" />\n\t\t\
    <meta name=\"viewport\" content=\"width=device-width\" /><link href='https://fonts.googleapis.com/css?family=Advent+Pro' rel=\"stylesheet\" type=\"text/css\"><style>\n\
    <style>\n\t\
    div {text-align: center; font-family: 'Advent Pro';font-weight: 300;left: 50%;position: absolute;top: 10%;transform: translateX(-50%) translateY(-50%);}\n\
    h1,h4 {text-color: white; font-size: 25px; text-align: center; margin-top: 1; margin-left: auto; margin-right: auto; }\n\t\
    h2 {text-color: white; font-size: 20px; margin-top: 0; margin-left: auto; margin-right: auto; font-weight: 180; text-align: center;}\n\t\
      body {background-color: darkgrey; height: 100%; margin-right: 50px; margin-left: 50px;}\n\t\t\
    </style>\n\t\
        <script src='\https://cdnjs.cloudflare.com/ajax/libs/jquery/3.2.1/jquery.js'\></script>\n\t\t\
    <script type=\'text/javascript\'>\n\t\
        wuri='\http://dataservice.accuweather.com/currentconditions/v1/2956111?apikey=4mMBpfdcAAmaMFXtwPvYK9qS9Wzq0md9'\;\n\t\t\
        $.getJSON(wuri, function (json) {\n\
           document.getElementById(\'city\').innerHTML = json[0].Temperature.Metric.Value +\' &deg;C\'\ \n\
          });\n\
    </script>\n\
    <script type=\'text/javascript\'>\n\t\
    function startUp(){\n\t\
    window.scrollTo(0, 1);\n\t\
    fetchData();\n\t\
    }\n\
    url = \document.location.href + 'json\';\n\t\t\
  setInterval(function() {\n\t\t\
  fetchData();\n\
  }, 10000);\n\
  function fetchData(){\n\t\
  var xmlhttp = new XMLHttpRequest();\n\t\
  xmlhttp.open(\'GET\', url, true);\n\t\
  xmlhttp.onreadystatechange = function() {\n\t\
      if (xmlhttp.readyState == 4) {\n\t\
          if(xmlhttp.status == 200) {\n\
              var obj = JSON.parse(xmlhttp.responseText);\n\
              if (obj.temperature < 70){\n\t\
        document.getElementById(\'temp\').innerHTML = parseFloat(Math.round(obj.temperature * 100) /\ 100).toFixed(2) +\' &deg;C\';\n\
        document.getElementById(\'dp\').innerHTML = parseFloat(Math.round(obj.dewpoint * 100) /\ 100).toFixed(1) +\' &deg;C\';\n\
        document.getElementById(\'hic\').innerHTML = parseFloat(Math.round(obj.heatindex * 100) /\ 100).toFixed(1) +\' &deg;C\';\n\
        document.getElementById(\'hum\').innerHTML = parseFloat(Math.round(obj.humidity * 100) /\ 100).toFixed(1) +\'%\';\n\t}\n\
           }\n\
      }\n\
  };\n\
  xmlhttp.send(null);\n\
  }\n\
    </script>\n\
    \n\t\n</head>\n\
    <body onload=\'startUp()\'>\n\
    <\div align='\center'\>Welcome to Sanjeev, Nitin and Krysto's room in Phillaur.</\div>\n\
    <h1>Outside Temperature</h1>\n\
    <\h2 id=\'city\'></\h2>\n\
    <h1>Room Temperature</h1>\n\
    <\h2 id=\'temp\'></\h2>\n\
    <h1>Dewpoint</h1>\n\
    <\h2 id=\'dp\'></\h2>\n\
    <h1>Heat Index</h1>\n\
    <\h2 id=\'hic\'></\h2>\n\
    <h1>Humidity</h1>\n\
    <\h2 id=\'hum\'></\h2>\n\t";
}

