import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';


void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  // This widget is the root of your application.

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Flutter Demo',
      theme: ThemeData(
        primarySwatch: Colors.green,
        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
      home: MyHomePage(title: 'Parking'),
    );
  }
}

class MyHomePage extends StatefulWidget {
  MyHomePage({Key key, this.title}) : super(key: key);

  final String title;

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  final databaseReference = FirebaseDatabase.instance.reference();
  DatabaseReference espRef =
  FirebaseDatabase.instance.reference().child('Esp');

  double ledOn = 0;

  double ledOff = 0;
  String uidIn="";
  String uidOut="";

  @override
  void initState() {
    super.initState();

    databaseReference.child("Esp").once().then((DataSnapshot snapshot) {
      ledOn = snapshot.value['ledStatus']['ledOn'].toDouble();
      ledOff = snapshot.value['ledStatus']['ledOff'].toDouble();

      uidIn = snapshot.value['rfid']['in'];
      uidOut = snapshot.value['rfid']['out'];
print(uidIn);
    });

    espRef.child('rfid').child('in').onValue.listen((event) {
      print(event.snapshot.value.toString());

      if (event.snapshot.value.toString() == '84a2132d') {
        _turnOnLed();

      }else _turnOffLed();
    });


  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title,style: TextStyle(fontSize: 30),),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[

          ],
        ),
      ),
      // This trailing comma makes auto-formatting nicer for build methods.
    );
  }

  void _turnOnLed() {
    databaseReference.child("Esp/ledStatus").update({
      'ledOn': 1,
    });

    databaseReference.child("Esp/ledStatus").update({
      'ledOff': 0,
    });

    setState(() {
      ledOn =1;
      ledOff =0;
    });
  }

  void _turnOffLed() {
    databaseReference.child("Esp/ledStatus").update({
      'ledOff': 1,
    });

    databaseReference.child("Esp/ledStatus").update({
      'ledOn': 0,
    });

    setState(() {
      ledOn =0;
      ledOff =1;
    });
  }
}