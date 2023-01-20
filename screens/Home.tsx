import { useEffect, useState } from 'react';
import { Alert, StyleSheet, Text, View, Pressable, TextInput, FlatList } from 'react-native';
import { Indicator } from '../components/Indicator';
import { Button } from '../components/Button';
import { SafeAreaView, Modal } from 'react-native';
import { useNavigation, useRoute } from '@react-navigation/native';
import type { StackNavigationProp } from '@react-navigation/stack';
import type { RootStackParamList } from '../utils/routes';
import { ref, onValue, set,child, push, remove, get } from "firebase/database";
import startFirebase from '../firebase-config';

type Data = { 
  Humidity1: number,
  SM1: number,
  Heat1: number,
  LDR1: number,
  Humidity2: number,
  SM2: number,
  Heat2: number,
  LDR2: number

}

export default function Home() {
  const navigation = useNavigation<StackNavigationProp<RootStackParamList>>();
  const [fetchTime, setFetchTime] = useState(true)
  const id = parseInt(useRoute().name.charAt(4));
  const scheduleId = id === 1 ? 'Schedule1' : 'Schedule2';
  const db = startFirebase()
  const dbRef = ref(db, 'PayStat');
  const [modalVisible, setModalVisible] = useState(false);
  const [period, setPeriod] = useState('10');
  const [fertilizer, setFertilizer] = useState('1');
  const [stat, setStat] = useState({});
  const onChanged = (text: string, isPeriod: boolean ) => {
    let newText = '';
    let numbers = '0123456789';

    for (var i=0; i < text.length; i++) {
        if(numbers.indexOf(text[i]) > -1 ) {
            newText = newText + text[i];
        }
        else {
            alert("please enter numbers only");
        }
    }
    isPeriod? setPeriod(newText) : setFertilizer(newText);
    //handleData(id, {time: date, period: period, fertilizer: fertilizer})
  }
  let data:Data = {
    Humidity1: 0, SM1: 0, Heat1: 0, LDR1: 0,
    Humidity2: 0, SM2: 0, Heat2: 0, LDR2: 0
  }
  
  onValue(dbRef, (snapshot) => {
    data = (snapshot.val());
      });
  const scheduleRef = ref(db, `Pay${id}`);
  let mode = 'Automatic';
  let scheduleData: readonly any[] | null | undefined = []
  onValue(scheduleRef, (snapshot) => {
    mode = (snapshot.val().Mode);
    scheduleData = (snapshot.val().data);
      });
  
  const handleIrrigateNow = () => {
    const IrrigatnowRef = ref(db, `irrigateNow${id}`);
    set(IrrigatnowRef,
      {endHour: new Date(Date.now()+parseInt(period)*60000).getHours(),
       endMinute: new Date(Date.now()+parseInt(period)*60000).getMinutes(),
       fertilizer: parseInt(fertilizer),
      });
    setModalVisible(false);
  }
  useEffect(() => {
    const interval = setInterval(() => {
      onValue(dbRef, (snapshot) => {
        data = (snapshot.val());
          });
      setFetchTime(!fetchTime)
    }, 100);
    return () => clearInterval(interval);
  },[fetchTime])
  return (
    <SafeAreaView style={styles.container}>
      <Text>{navigation.getId()}</Text>
      <View style={styles.row}>
        <Indicator type="temprature" value={id ==1 ?data.Heat1 : data.Heat2} />
        <Indicator type="humidity" value={id ==1 ?data.Humidity1 : data.Humidity2} />
      </View>
      <View style={styles.row}>
        <Indicator type="soil" value={id ==1 ?data.SM1 : data.SM2} />
        <Indicator type="light" value={id ==1 ?data.LDR1 : data.LDR2} />
      </View>
      <Button text="Irrigate Now" onPress={() => setModalVisible(true)}/>
      <Modal
        animationType="slide"
        transparent={true}
        visible={modalVisible}
        onRequestClose={() => {
          Alert.alert('Modal has been closed.');
          setModalVisible(!modalVisible);
        }}>
        <View style={styles.centeredView}>
          <View style={styles.modalView}>
          <View style={styles.textInputcontainer}>
            <Text>Period: </Text>
            <TextInput
              keyboardType='numeric'
              onChangeText={text => onChanged(text, true)}
              value={period}
              style={styles.input}
              placeholder='Minuites'
              maxLength={10}
            />
          </View>
  
          <View style={styles.textInputcontainer}>
            <Text>Fertilizer: </Text>
            <TextInput
              keyboardType='numeric'
              onChangeText={text => onChanged(text, false)}
              value={fertilizer}
              style={styles.input}
              placeholder='Litres'
              maxLength={10}
            />
          </View>
            <Pressable
              style={[styles.button, styles.buttonClose]}
              onPress={() => handleIrrigateNow()}>
              <Text >Submit</Text>
            </Pressable>
          </View>
        </View>
      </Modal>
      <Button text="schedule" onPress={() => navigation.navigate(scheduleId, {id: parseInt(navigation.getId()?.charAt(4) as string)})} />
      <View style={styles.schedule}>
        <Text>Current Schedule</Text>
        <Text>{mode}</Text>
        <FlatList
          scrollEnabled
          
          data={scheduleData}
          renderItem={({ item }) => 
          <>
          <View style={styles.scheduleText}>
            <Text>time: {item.data.hour}:{item.data.minute}   period:{(item.data.endHour*60 + item.data.endMinute) - (item.data.hour*60 + item.data.minute) } minutes  fertilizer:{item.data.fertilizer} Liter</Text>
          </View>
          </>
          
        }
          keyExtractor={item => item.time}
        />
      </View>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
    alignItems: 'center',
  },
  row: {
    flexDirection: 'row',
    marginBottom: 20,
  },
  centeredView: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    marginTop: 22,
  },
  modalView: {
    margin: 20,
    backgroundColor: 'white',
    borderRadius: 20,
    padding: 35,
    width: '80%',
    alignItems: 'center',
    shadowColor: '#000',
    
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.25,
    shadowRadius: 4,
    elevation: 5,
  },
  button: {
    borderRadius: 20,
    padding: 10,
    elevation: 2,
  },
  
  buttonClose: {
    backgroundColor: 'green',
  },
  input: {
    borderWidth:1,
    borderColor:'green',
    borderRadius:10,
    width: '50%',
    padding:10,
  },
  textInputcontainer: {
    width: '100%',
    flexDirection: "row",
    justifyContent: "space-around",
    alignItems: "center",
    paddingBottom: 10,
},
  schedule:{
    marginTop: 20,
    justifyContent: 'center',
    alignItems: 'center',
  },
  scheduleText:{
    backgroundColor: 'green',
    color: 'white',
    textDecorationColor: 'white',
    padding: 10,
    borderRadius: 10,
    marginTop: 10,
    marginBottom: 10,
  },
  scheduleList:{
    borderRadius: 10,
    borderStyle: 'solid',
    borderWidth: 1,
  }

});
