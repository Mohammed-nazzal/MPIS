import React from 'react'
import { View, Text, StyleSheet } from 'react-native'
import Celsius from '../assets/icons/celsius.svg'
import Temperature from '../assets/icons/temperature.svg'
import WaterDrop from '../assets/icons/water-drop.svg'
import Percentage from '../assets/icons/percentage.svg'
import SoilMoisture from '../assets/icons/soil-moisture.svg'
import Sun from '../assets/icons/sun.svg'
const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent:'center', 
        alignItems:'center',
        backgroundColor: '#fff',
        paddingVertical: 20,
        borderRadius: 20,
        
    },
    dropShadow:{
        shadowColor: 'rgba(87, 207, 45, 0.91)',
        shadowOffset: {
            width: 0,
            height: 0,
        },
        shadowOpacity: 100,
        shadowRadius: 10.32,
        elevation: 1,
        borderWidth:0,
        borderRadius:10,
        height: 150,
        width: 150,
        margin: 10,

        
    },
    row:{
        flexDirection: 'row',
        width: '100%',
        backgroundColor: 'white',
        marginVertical: 10,
        justifyContent: 'space-around',

    }
});


type IndicatorProps = {
    type : 'temprature' | 'humidity' | 'light' | 'soil',
    value : number,
}
export const Indicator = ({type = 'temprature', value=25}: IndicatorProps) => {
    const color = type === 'temprature' ? 'rgba(87, 207, 45, 0.91)' : type === 'humidity' ? 'rgba(19, 193, 193, 0.91)' : type === 'light' ? '#DECB35' : '#B82DCF';
    return(
        
          <View style={[styles.dropShadow,{shadowColor: color,}]}>
            <View style={styles.container}>
              <View style={styles.row}>
                  {type === 'temprature' && <Temperature width={50} height={50} />}
                  {type === 'humidity' && <WaterDrop width={50} height={50} />}
                  {type === 'light' && <Sun width={50} height={50} />}
                  {type === 'soil' && <SoilMoisture width={50} height={50} fill='#000'/>}
              </View>
              <View style={styles.row}>
                  <Text style={{fontSize:36, fontWeight:'bold'}}>{value}</Text>
                  {type === 'temprature' && <Celsius width={50} height={50} />}
                  {type === ('humidity'|| 'light' || 'soil') && <Percentage width={50} height={50} />}
                  
              </View>
              </View>
          </View>
            
           
        
    )       
    }
