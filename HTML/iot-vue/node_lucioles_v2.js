// Importation des modules
var path = require("path");

// var, const, let :
// https://medium.com/@vincent.bocquet/var-let-const-en-js-quelles-diff%C3%A9rences-b0f14caa2049

const mqtt = require("mqtt");
// Topics MQTT
const TOPIC_LED = "sensors/led";
const TOPIC_LIGHT = "sensors/light";
const TOPIC_TEMPERATURE = "sensors/temp";
const TOPIC_TEMP_TRESHOLD_1 = "tresholds/temp1";
const TOPIC_TEMP_TRESHOLD_2 = "tresholds/temp2";
const TOPIC_LIGHT_TRESHOLD_1 = "tresholds/light1";
const TOPIC_LIGHT_TRESHOLD_2 = "tresholds/light2";
const TOPIC_SLEEP_TIME_1 = "sleep_times/regime1";
const TOPIC_SLEEP_TIME_2 = "sleep_times/regime2";
const TOPIC_WORKING_HOURS_START_1 = "working_hours/start1";
const TOPIC_WORKING_HOURS_END_1 = "working_hours/end1";
const TOPIC_WORKING_HOURS_START_2 = "working_hours/start2";
const TOPIC_WORKING_HOURS_END_2 = "working_hours/end2";
const TOPIC_ALERT_TEMP = "alerts/temp";
const TOPIC_ALERT_LIGHT = "alerts/light";

// Mailing
const nodemailer = require("nodemailer");
process.env.NODE_TLS_REJECT_UNAUTHORIZED = "0";

// express
const express = require("express");
const bodyParser = require("body-parser");

const app = express();
//Pour permettre de parcourir les body des requetes
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
app.use(express.static(path.join(__dirname, "/")));
app.use(function (request, response, next) {
  //Pour eviter les problemes de CORS/REST
  response.header("Access-Control-Allow-Origin", "*");
  response.header("Access-Control-Allow-Headers", "*");
  response.header(
    "Access-Control-Allow-Methods",
    "POST, GET, OPTIONS, PUT, DELETE"
  );
  next();
});

// MongoDB
var mongodb = require("mongodb");
const mongoBaseName = "lucioles"; // Nom de la base
//const uri = 'mongodb://localhost:27017/'; //URL de connection
//const uri = 'mongodb://10.9.128.189:27017/'; //URL de connection
const uri = "mongodb://localhost:27017/";
//"mongodb+srv://menez:mettrelevotre@cluster0-x0zyf.mongodb.net/test?retryWrites=true&w=majority";

const MongoClient = require("mongodb").MongoClient;
const client = new MongoClient(uri, {
  useNewUrlParser: true,
  useUnifiedTopology: true,
});

// Connection a la DB MongoDB
client.connect(function (err, mongodbClient) {
  if (err) throw err; // If connection to DB failed ...
  // else we get a "db" engine reference

  //===============================================
  // Get a connection to the DB "lucioles" or create
  //
  var dbo = client.db(mongoBaseName);

  /* dbo.dropCollection("temp", function (err, delOK) {
    if (err) throw err;
    if (delOK) console.log("Collection deleted");
  });

  dbo.dropCollection("light", function (err, delOK) {
    if (err) throw err;
    if (delOK) console.log("Collection deleted");
  }); */
  //===============================================
  // Connection au broker MQTT distant
  //
  //const mqtt_url = 'http://192.168.1.100:1883' ///134.59.131.45:1883'
  const mqtt_url = "http://212.115.110.52:1818";
  var client_mqtt = mqtt.connect(mqtt_url);

  //===============================================
  // Des la connection, le serveur NodeJS s'abonne aux topics MQTT
  //
  client_mqtt.on("connect", function () {
    client_mqtt.subscribe(TOPIC_LIGHT, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_LIGHT, 'Hello mqtt')
        console.log("Node Server has subscribed to ", TOPIC_LIGHT);
      }
    });

    client_mqtt.subscribe(TOPIC_TEMPERATURE, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_TEMPERATURE, 'Hello mqtt')
        console.log("Node Server has subscribed to ", TOPIC_TEMPERATURE);
      }
    });

    client_mqtt.subscribe(TOPIC_LED, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_LED, 'Hello mqtt')
        console.log("Node Server has subscribed to ", TOPIC_LED);
      }
    });

    client_mqtt.subscribe(TOPIC_TEMP_TRESHOLD_1, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_TEMP_TRESHOLD_1, 'Hello mqtt')
        console.log("Node Server has subscribed to ", TOPIC_TEMP_TRESHOLD_1);
      }
    });

    client_mqtt.subscribe(TOPIC_TEMP_TRESHOLD_2, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_TEMP_TRESHOLD_2, 'Hello mqtt')
        console.log("Node Server has subscribed to ", TOPIC_TEMP_TRESHOLD_2);
      }
    });

    client_mqtt.subscribe(TOPIC_LIGHT_TRESHOLD_1, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_LIGHT_TRESHOLD_1, 'Hello mqtt')
        console.log("Node Server has subscribed to ", TOPIC_LIGHT_TRESHOLD_1);
      }
    });

    client_mqtt.subscribe(TOPIC_LIGHT_TRESHOLD_2, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_LIGHT_TRESHOLD_2, 'Hello mqtt')
        console.log("Node Server has subscribed to ", TOPIC_LIGHT_TRESHOLD_2);
      }
    });

    client_mqtt.subscribe(TOPIC_SLEEP_TIME_1, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_SLEEP_TIME_1, 'Hello mqtt')
        console.log("Node Server has subscribed to ", TOPIC_SLEEP_TIME_1);
      }
    });
    client_mqtt.subscribe(TOPIC_SLEEP_TIME_2, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_SLEEP_TIME_2, 'Hello mqtt')
        console.log("Node Server has subscribed to ", TOPIC_SLEEP_TIME_2);
      }
    });

    client_mqtt.subscribe(TOPIC_WORKING_HOURS_START_1, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_WORKING_HOURS_START_1, 'Hello mqtt')
        console.log(
          "Node Server has subscribed to ",
          TOPIC_WORKING_HOURS_START_1
        );
      }
    });
    client_mqtt.subscribe(TOPIC_WORKING_HOURS_END_1, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_WORKING_HOURS_END_1, 'Hello mqtt')
        console.log(
          "Node Server has subscribed to ",
          TOPIC_WORKING_HOURS_END_1
        );
      }
    });
    client_mqtt.subscribe(TOPIC_WORKING_HOURS_START_2, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_WORKING_HOURS_START_2, 'Hello mqtt')
        console.log(
          "Node Server has subscribed to ",
          TOPIC_WORKING_HOURS_START_2
        );
      }
    });

    client_mqtt.subscribe(TOPIC_WORKING_HOURS_END_2, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_WORKING_HOURS_END_2, 'Hello mqtt')
        console.log(
          "Node Server has subscribed to ",
          TOPIC_WORKING_HOURS_END_2
        );
      }
    });

    client_mqtt.subscribe(TOPIC_ALERT_TEMP, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_ALERT_TEMP, 'Hello mqtt')
        console.log("Node Server has subscribed to ", TOPIC_ALERT_TEMP);
      }
    });

    client_mqtt.subscribe(TOPIC_ALERT_LIGHT, function (err) {
      if (!err) {
        //client_mqtt.publish(TOPIC_ALERT_LIGHT, 'Hello mqtt')
        console.log("Node Server has subscribed to ", TOPIC_ALERT_LIGHT);
      }
    });
  });

  app.post("/sendRegime", function (request, response) {
    console.log("POST /");
    console.dir(request.body);

    if (request.body.id == 1) {
      client_mqtt.publish(TOPIC_TEMP_TRESHOLD_1, request.body.seuilTemp);
      client_mqtt.publish(TOPIC_LIGHT_TRESHOLD_1, request.body.seuilLumin);
      client_mqtt.publish(TOPIC_SLEEP_TIME_1, request.body.sleepTime);
      client_mqtt.publish(
        TOPIC_WORKING_HOURS_START_1,
        request.body.beginingRegime
      );
      client_mqtt.publish(TOPIC_WORKING_HOURS_END_1, request.body.endRegime);
    } else if (request.id == 2) {
      client_mqtt.publish(TOPIC_TEMP_TRESHOLD_2, request.body.seuilTemp);
      client_mqtt.publish(TOPIC_LIGHT_TRESHOLD_2, request.body.seuilLumin);
      client_mqtt.publish(TOPIC_SLEEP_TIME_2, request.body.sleepTime);
      client_mqtt.publish(
        TOPIC_WORKING_HOURS_START_2,
        request.body.beginingRegime.toString()
      );
      client_mqtt.publish(
        TOPIC_WORKING_HOURS_END_2,
        request.body.endRegime.toString()
      );
    }
    response.writeHead(200, { "Content-Type": "text/html" });
    response.end("thanks");
  });

  //================================================================
  // Callback de la reception des messages MQTT pour les topics sur
  // lesquels on s'est inscrit.
  // C'est cette fonction qui alimente la BD.
  //
  client_mqtt.on("message", function (topic, message) {
    console.log("MQTT msg on topic : ", topic.toString());
    console.log("Msg payload : ", message.toString());

    // Parsing du message supposé recu au format JSON
    message = JSON.stringify(message);
    message = JSON.parse(message);
    wh = message.who;
    val = message.value;

    console.log(topic);

    //Envoi du mail en fonction de l'alerte reçue
    // VARIABLES Mailing
    let transporter = nodemailer.createTransport({
      host: "smtp.gmail.com",
      port: 587,
      secure: false,
      requireTLS: true,
      auth: {
        user: "alert.lucioles@gmail.com",
        pass: "iotM1Miage",
      },
    });

    let mailAlerteLuminosite = {
      from: "alert.lucioles@gmail.com",
      to: "redadu06.rj@gmail.com",
      subject: "Test",
      text:
        "Bonjour, \n Ceci est une alerte de luminosté. La luminosité est de " +
        val +
        "\n Une ou plusieurs lampes de votre domicile sont allumées alors qu elles ne devraient pas l être",
    };

    let mailAlerteTemperature = {
      from: "alert.lucioles@gmail.com",
      to: "redadu06.rj@gmail.com",
      subject: "Test",
      text:
        "Bonjour,  \n Ceci est une alerte de température. La température est de " +
        val +
        "°C. \n La température de votre domicile dépasse la normale, veuillez vérifier cette dernière",
    };

    if (topic == "alerts/temp") {
      transporter.sendMail(mailAlerteTemperature, (error, info) => {
        if (error) {
          return console.log(error.message);
        }
        console.log("Envoi mail alerte temperature");
      });
    }

    if (topic == "alerts/light") {
      transporter.sendMail(mailAlerteLuminosite, (error, info) => {
        if (error) {
          return console.log(error.message);
        }
        console.log("Envoi mail alerte luminosite");
      });
    }

    // Debug : Gerer une liste de who pour savoir qui utilise le node server
    let wholist = [];
    var index = wholist.findIndex((x) => x.who == wh);
    if (index === -1) {
      wholist.push({ who: wh });
    }
    console.log("wholist using the node server :", wholist);

    // Mise en forme de la donnee à stocker => dictionnaire
    // Le format de la date est iomportant => copatible avec le
    // parsing qui sera realise par hightcharts dans l'UI
    // cf https://www.w3schools.com/jsref/tryit.asp?filename=tryjsref_tolocalestring_date_all
    // vs https://jsfiddle.net/BlackLabel/tgahn7yv
    //var frTime = new Date().toLocaleString("fr-FR", {timeZone: "Europe/Paris"});
    var frTime = new Date().toLocaleString("sv-SE", {
      timeZone: "Europe/Paris",
    });
    var new_entry = {
      date: frTime, // timestamp the value
      who: wh, // identify ESP who provide
      value: val, // this value
    };

    // On recupere le nom du topic du message
    var topicname = path.parse(topic.toString()).base;

    // Stocker la donnee/value contenue dans le message en
    // utilisant le nom du topic comme key dans la BD
    key = topicname;
    dbo.collection(key).insertOne(new_entry, function (err, res) {
      if (err) throw err;
      console.log("Item inserted in db in collection :", key);
      console.log(new_entry);
    });

    // Debug : voir les collections de la DB
    dbo.listCollections().toArray(function (err, collInfos) {
      // collInfos is an array of collection info objects that look like:
      // { name: 'test', options: {} }
      console.log("\nList of collections currently in DB: ", collInfos);
    });
  }); // end of 'message' callback installation

  //================================================================
  // Fermeture de la connexion avec la DB lorsque le NodeJS se termine.
  //
  process.on("exit", (code) => {
    if (mongodbClient && mongodbClient.isConnected()) {
      console.log("mongodb connection is going to be closed ! ");
      mongodbClient.close();
    }
  });

  //================================================================
  //==== REQUETES HTTP reconnues par le Node =======================
  //================================================================
  //app.use("/static", express.static(__dirname + "/public"));
  // Accés par le Node a la page HTML affichant les charts
  app.get("/", function (req, res) {
    res.sendFile(path.join(__dirname + "/public/index.html"));
  });

  // Function for answering GET request on this node server ...
  // probably from navigator.
  // The request contains the name of the targeted ESP !
  //     /esp/temp?who=80%3A7D%3A3A%3AFD%3AC9%3A44
  // Utilisation de routes dynamiques => meme fonction pour
  // /esp/temp et /esp/light
  app.get("/esp/:what", function (req, res) {
    // cf https://stackabuse.com/get-query-strings-and-parameters-in-express-js/
    console.log(req.originalUrl);

    wh = req.query.who; // get the "who" param from GET request
    // => gives the Id of the ESP we look for in the db
    wa = req.params.what; // get the "what" from the GET request : temp or light ?

    console.log("\n--------------------------------");
    console.log("A client/navigator ", req.ip);
    console.log("sending URL ", req.originalUrl);
    console.log("wants to GET ", wa);
    console.log("values from object ", wh);

    const nb = 200; // Récupération des nb derniers samples
    // stockés dans la collection associée a ce
    // topic (wa) et a cet ESP (wh)
    key = wa;
    //dbo.collection(key).find({who:wh}).toArray(function(err,result) {
    dbo
      .collection(key)
      .find({ who: wh })
      .sort({ _id: -1 })
      .limit(nb)
      .toArray(function (err, result) {
        if (err) throw err;
        console.log("get on ", key);
        console.log(result);
        res.json(result.reverse()); // This is the response.
        console.log("end find");
      });
    console.log("end app.get");
  });
}); // end of MongoClient.connect

// L'application est accessible sur le port 3000
app.listen(3000, () => {
  console.log("Server listening on port 3000");
});

app.post("/s", (req, res) => {
  gm(img_path)
    .implode(-1.2)
    .write(op_path, function (err) {
      if (err) console.log(err);
    });
});
