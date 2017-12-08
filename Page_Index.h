//
//   The HTML PAGE
//
const char PAGE_Index[] PROGMEM = R"=====(
<html>
  <head>
    <title>Krysto San Nitin's Room</title>
    <meta http-equiv='Content-Type' content='text/html;charset=utf-8' />
    <meta name="apple-mobile-web-app-capable" content="yes" />
    <meta name="viewport" content="width=device-width" /><link href='https://fonts.googleapis.com/css?family=Advent+Pro' rel="stylesheet" type="text/css"><style>
    <style>
    div {text-align: center; font-family: 'Advent Pro';font-weight: 300;left: 50%;position: absolute;top: 10%;transform: translateX(-50%) translateY(-50%);}
    h1,h4 {text-color: white; font-size: 25px; text-align: center; margin-top: 1; margin-left: auto; margin-right: auto; }
    h2 {text-color: white; font-size: 20px; margin-top: 0; margin-left: auto; margin-right: auto; font-weight: 180; text-align: center;}
      body {background-color: darkgrey; height: 100%; margin-right: 50px; margin-left: 50px;}
    </style>
        <script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/3.2.1/jquery.js'></script>

    <script type='text/javascript'>
        wuri='http://dataservice.accuweather.com/currentconditions/v1/2956111?apikey=4mMBpfdcAAmaMFXtwPvYK9qS9Wzq0md9';
        $.getJSON(wuri, function (json) {
         //  Disabled as we have sensor plugged in.
           // document.getElementById('city').innerHTML = json[0].Temperature.Metric.Value +' &deg;C' 
          });
    </script>
    <script type='text/javascript'>
    function startUp(){
    window.scrollTo(0, 1);
    fetchData();
    }
    surl = window.location.origin + "/json";
  setInterval(function() {
  fetchData();
  }, 10000);
  function fetchData(){
  var xmlhttp = new XMLHttpRequest();
  xmlhttp.open('GET', surl, true);
  xmlhttp.onreadystatechange = function() {
      if (xmlhttp.readyState == 4) {
          if(xmlhttp.status == 200) {
            console.log(xmlhttp.responseText);
              var obj = JSON.parse(xmlhttp.responseText);
              if (obj.temperature < 70){
//        var x = (false)?("1true"):((true)?"2true":"2false");
        out = document.getElementById('city')
        out.innerHTML =  parseFloat(Math.round(obj.Outtemperature * 100) / 100).toFixed(2) + ' &deg;C';
        document.getElementById('temp').innerHTML = parseFloat(Math.round(obj.temperature * 100) / 100).toFixed(2) + ' &deg;C';
        document.getElementById('dp').innerHTML = parseFloat(Math.round(obj.dewpoint * 100) / 100).toFixed(1) +' &deg;C';
        document.getElementById('hic').innerHTML = parseFloat(Math.round(obj.heatindex * 100) / 100).toFixed(1) + ' &deg;C';
        document.getElementById('hum').innerHTML = parseFloat(Math.round(obj.humidity * 100) / 100).toFixed(1) +' %';
        document.getElementById('doormsg').innerHTML =  obj.doorOpen == 1 ? "Door is open." :  "";
        tempf = (obj.Outtemperature * 1.8)+32;
        wupUrl = "https://rtupdate.wunderground.com/weatherstation/updateweatherstation.php?ID=IPHILLAU2&PASSWORD=9ocfq6ge&dateutc=2000-01-01+10%3A32%3A35&tempf="+tempf+"&humidity=90&action=updateraw&realtime=1&rtfreq=2.5"
         $.getJSON(wupUrl, function (json) {
         //  Disabled as we have sensor plugged in.
           // document.getElementById('city').innerHTML = json[0].Temperature.Metric.Value +' &deg;C' 
          });
        }
           }
      }
  };
  xmlhttp.send(null);
  }
    </script>
    </head>
    <body onload='startUp()'>
  
    <div id="welMsg" align='center'>...</div>
    <script type='text/javascript'>
    function getUrlVars() {
    var vars = {};
    var parts = window.location.href.replace(/[?&]+([^=&]+)=([^&]*)/gi,    
    function(m,key,value) {
      vars[key] = value;
    });
    return vars;
    }

    usr = getUrlVars()["user"];
    if (!usr){
      document.getElementById('welMsg').innerHTML = "Welcome to Sanjeev, Nitin and Krysto's room in Phillaur.";
    }
    else {
      document.getElementById('welMsg').innerHTML = "Hello " + usr + "," ;
    }
    </script>
    <h1>Outside Temperature</h1>
    <h2 id='city'></h2>
    <!--p id='outSens'></p-->
    <h1>Room Temperature</h1>
    <h2 id='temp'></h2>
    <h1>Dewpoint</h1>
    <h2 id='dp'></h2>
    <h1>Heat Index</h1>
    <h2 id='hic'></h2>
    <h1>Humidity</h1>
    <h2 id='hum'></h2>
    <p id='doormsg'></p>
    </\div>
    </body>
</html>
)=====" ;

