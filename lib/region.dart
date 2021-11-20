import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';

import 'main.dart';

class region extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false, //لإخفاء شريط depug
      home: Scaffold(
        backgroundColor: ba1color,
        appBar: AppBar(
          automaticallyImplyLeading: true, //false
          //actions: <Widget>[new Container(),],
          title: // Center(child:
              Text(
            "OtoBüs",
            style: TextStyle(
              fontSize: 25,
              fontFamily: 'Pacifico',
              color: Colors.white,
            ),
          ),
          //),
          backgroundColor: apcolor,
        ),
      ),
    );
    //body:
  }
}
