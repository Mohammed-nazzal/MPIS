import React, { useState } from "react";
import { View, Text, StyleSheet, TextInput } from "react-native";
import DateTimePicker from '@react-native-community/datetimepicker';

const styles = StyleSheet.create({
    mainContainer: {
        padding: 10,
        borderStyle: "solid",
        borderWidth: 1,
        borderColor: "#000000",
        // flexDirection: "row",
        borderRadius: 10,
        //overflow: "hidden",
        justifyContent: "space-between",
        marginBottom: 10,
        
        
    },
    container: {
        paddingBottom: 10,
        flexDirection: "row",
        justifyContent: "space-between",
        
    },
    input: {
        borderWidth:1,
        width:'70%',
        borderColor:'#c7c3c3',
        padding:10,
    },

})
type PickerProps = {
  id: number,
  handleData: (id: number, data: object)=> void,
  mode?: string,
    
}
 const Picker = ({id, handleData}: PickerProps) => {
    


    const [show, setShow] = useState(false);
    const [period, setPeriod] = React.useState('10');
    const [fertilizer, setFertilizer] = React.useState('0');
    const [date, setDate] = useState<Date>(new Date(Date.now() + id*720000));

    const onChange = (event: unknown, selectedDate: Date) => {
        const currentDate = selectedDate;
        setShow(false);
        setDate(currentDate);
        //handleData(id, {time: currentDate, period: period, fertilizer: fertilizer})
      };
      //handleData(key, {time: date, period: period, fertilizer: fertilizer})

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
    
    React.useEffect(() => {
      
      handleData(id, {
        hour: date.getHours(),
        minute: date.getMinutes(),
        endHour: new Date(date.getTime() + parseInt(period)*60000).getHours(),
        endMinute: new Date(date.getTime() + parseInt(period)*60000).getMinutes(),
        fertilizer : parseInt(fertilizer)
          }
        )
      //Alert.alert("useEffect", JSON.stringify({id: id, time: date, period: period, fertilizer: fertilizer}))
    }, [period, fertilizer, date])
    return (
        <View style={styles.mainContainer}>
        
          <View style={styles.container}>
              <Text>Time: </Text>
              <DateTimePicker
                testID="dateTimePicker"
                value={date}
                mode='time'
                is24Hour={true}
                onChange={onChange as unknown as (event: unknown, selectedDate: Date | undefined) => void}
              />
          </View>
          
          <View style={styles.container}>
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
  
          <View style={styles.container}>
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

        </View>
    )
 }

 export default Picker;