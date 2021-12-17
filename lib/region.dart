import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:percent_indicator/percent_indicator.dart';
import 'package:firebase_database/firebase_database.dart';

import 'main.dart';

double num = 0;

class region extends StatefulWidget {
  final String reg;
  region(this.reg);
  @override
  _regionState createState() => _regionState();
}

class _regionState extends State<region> {
  DatabaseReference regRef = FirebaseDatabase.instance.reference().child('Esp');
  String r = "";
  @override
  Widget build(BuildContext context) {
    // print(widget.reg);
    return MaterialApp(
      debugShowCheckedModeBanner: false, //لإخفاء شريط depug
      home: Scaffold(
          backgroundColor: ba1color,
          appBar: AppBar(
            leading: IconButton(
              icon: Icon(Icons.arrow_back, color: Colors.white),
              onPressed: () => Navigator.of(context).pop(),
            ),
            automaticallyImplyLeading: true, //false
            //actions: <Widget>[new Container(),],
            title: // Center(child:
                Text(
              "OtoBus", //r == "" ? "OtoBus" : r,
              style: TextStyle(
                fontSize: 25,
                fontFamily: 'Pacifico',
                color: Colors.white,
              ),
            ),
            //),
            backgroundColor: apcolor,
          ),
          body: ListView.builder(
            itemCount: inBus.length,
            itemBuilder: (context, i) {
              r = inBus[i].region;
              regRef
                  .child('region')
                  .child(inBus[i].region)
                  .child(inBus[i].uid)
                  .onValue
                  .listen((event) {
                setState(() {
                  inBus[i].taken = event.snapshot.value != null
                      ? event.snapshot.value.toDouble()
                      : 0;
                });
              });

              return inBus[i].region == widget.reg
                  ? Padding(
                      padding: EdgeInsets.all(8.0),
                      child: Card(
                        color: Color(0xFFe1f5e6),
                        child: ListTile(
                            // leading: FlutterLogo(size: 56.0),
                            title: Text(inBus[i].name),
                            subtitle: Text(inBus[i].busType),
                            trailing: CircularPercentIndicator(
                              radius: 50.0,
                              lineWidth: 5.0,
                              percent: inBus[i].taken /
                                  double.parse(inBus[i].passengers),
                              center: new Text(inBus[i].taken.toString()),
                              progressColor: apcolor,
                            )),
                      ),
                    )
                  : Container(
                      height: 0,
                      width: 0,
                    );
            },
          )),
    );
    //body:
  }
}
