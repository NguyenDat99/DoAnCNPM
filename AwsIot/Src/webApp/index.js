var awsIot = require('aws-iot-device-sdk');
var express = require('express');
var app = express();

var device = awsIot.device({
  keyPath: 'f79b16571c-private.pem.key',
  certPath: 'f79b16571c-certificate.pem.crt',
  caPath: 'AmazonRootCA1.pem',
  clientId: 'MyConnect',
  host: 'a2mbzfzdofwxgs-ats.iot.us-west-2.amazonaws.com'
});
app.set('view engine', 'ejs');
app.use(express.static(__dirname + '/public'));
app.listen(3000);

var action = process.openStdin();

action.addListener("data", function(d) {
  console.log(d.toString().trim());
});

device
  .on('connect', function(err, doc) {
    console.log('connect');
    device.subscribe('$aws/things/DoAnIotThing/shadow/update');
    device.publish('$aws/things/DoAnIotThing/shadow/update', JSON.stringify({
      "state": {
        "reported": {
          "temperature": 1111
        }
      }
    }));
  });

app.get('/', function(req, res) {
  device
    .on('connect', function(err, doc) {
      console.log('connect');
      device.subscribe('$aws/things/DoAnIotThing/shadow/update');
      device.publish('$aws/things/DoAnIotThing/shadow/update', JSON.stringify({
        "state": {
          "reported": {
            "temperature": 1111
          }
        }
      }));
    });
  var message = device
    .on('message', function(topic, payload) {
      var mess = payload;
      res.send(mess);
    });

});

device
  .on('message', function(topic, payload) {
    // console.log(payload.toString());
  });
