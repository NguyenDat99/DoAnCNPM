var awsIot = require('aws-iot-device-sdk');


var device = awsIot.device({
   keyPath: 'f79b16571c-private.pem.key',
  certPath: 'f79b16571c-certificate.pem.crt',
    caPath: 'AmazonRootCA1.pem',
  clientId: 'MyConnect',
   host: 'a2mbzfzdofwxgs-ats.iot.us-west-2.amazonaws.com'
});

device
  .on('connect', function() {
    console.log('connect');
    device.subscribe('$aws/things/DoAnIotThing/shadow/update');

    device.publish('$aws/things/DoAnIotThing/shadow/update', JSON.stringify({ test_data: 1}));

  });
  
device
  .on('message', function(topic, payload) {
    console.log(payload.toString());
  });
