import React, {useState} from "react"
import { View, Text, StyleSheet, SafeAreaView, FlatList, ScrollView } from "react-native"
import Checkbox from 'expo-checkbox';
import DropDownPicker from 'react-native-dropdown-picker'
import { useNavigation, useRoute } from "@react-navigation/native"
import Picker from "../components/Picker"
import { Button } from "../components/Button"
import type { StackNavigationProp } from "@react-navigation/stack"
import type { RootStackParamList } from "../utils/routes"
import startFirebase from "../firebase-config";
import { onValue, ref, set } from "firebase/database";
import { timeError } from "../utils/dateCompare";
import { LogBox } from 'react-native';


const styles = StyleSheet.create({
    container: {
        flex: 6,
        paddingVertical: 10,
        paddingHorizontal: 40,
        margin: 5,
        alignContent: "center",
    },
    checkboxContainer: {
        flex: 0.0,
        flexDirection: "row",
        alignItems: "center",
        marginBottom: 20,

    },
    periodsContainer: {
        flex: 1,
    },

    checkbox: {
        marginRight: 10,
    },
    button: {
        flex: 1,
        height: 50,
        alignItems: "center",
        justifyContent: "flex-end",
    }


})

type Data = {
    id: number,
    numOfDays?: number,
    day?: number,
    data: {
        hour: number,
        minute: number,
        endHour: number,
        endMinute: number,
        fertilizer: number,
    },
}

const Schedule = () => {
    const db = startFirebase()
    let dbRef = ref(db, 'Pay')
    const navigation = useNavigation<StackNavigationProp<RootStackParamList>>();
    const id = parseInt(useRoute().name.charAt(8))
    const [selected, setSelection] = useState({
        Automatic: true,
        EveryDay: false,
        EveryNumDays: false,
        EveryMonth: false,
    })

    const [data, setData] = useState<Array<Data>>([
        
    ])


    const [open, setOpen] = useState(false);
    const [value, setValue] = useState(0);
    const [items, setItems] = useState([
        {label: '1', value: 1},
        {label: '2', value: 2},
        {label: '3', value: 3},
        {label: '4', value: 4},
        {label: '5', value: 5},
        {label: '6', value: 6},
        {label: '7', value: 7},
        {label: '8', value: 8},
        {label: '9', value: 9},
        {label: '10', value: 10},
    ]);
    const [openNumDAys, setOpenNumDays] = useState(false);
    const [valueNumDays, setValueNumDays] = useState(0);
    const [itemsNumDays, setItemsNumDays] = useState([
        {label: '1', value: 1},
        {label: '2', value: 2},
        {label: '3', value: 3},
        {label: '4', value: 4},
        {label: '5', value: 5},
        {label: '6', value: 6},
        {label: '7', value: 7},
        {label: '8', value: 8},
        {label: '9', value: 9},
        {label: '10', value: 10},
    ]);
    let exist:boolean
    const selectMode = (mode: string) => {
        const select = {...selected}
        setSelection(() => {
            
            for(let key in selected){
                if(key === mode){
                    select[key as keyof typeof selected] = true
                }else{
                    select[key as keyof typeof selected] = false
                }
            }
            return select
        })
    }
    type renderData = {
        item: Data
    }
    const renderItems= ({item} : renderData) => {
        
        return(
        <>
        <Picker
          id={item.id}
          handleData={( index, elementData: any) => {
            setData((prevData) => {
                const newData = prevData.map((itemData) => {
                    if(itemData.id === index){
                        return {
                            ...itemData,
                            data: elementData
                        }
                    }else{
                        return itemData
                    }
                })
                return newData
            })
          }}
        />
        </>
        )
    }
    const extractDataObj = (data: Array<number>) => {
        const dataObj = data.map((item) => {
            return {
                id: item,
                data: {
                    hour: (new Date(Date.now() + item*720000)).getHours(),
                    minute: (new Date(Date.now() + item*720000)).getMinutes(),
                    endHour: new Date(Date.now() + 10*60000).getHours(),
                    endMinute: new Date(Date.now() + 10*60000).getMinutes(),
                    fertilizer: 1,
                }
            }
        })
        return dataObj
    }
    React.useEffect(() => {
        
        
        if(selected.EveryDay ){
            
            const dataObj = extractDataObj(Array.from(Array(value).keys()))
            setData(dataObj)
        }
        if(selected.EveryNumDays ){
            
            
            const dataObj = [{
                id: 0,
                data: {
                    numOfDays: valueNumDays,
                    day: (new Date(Date.now() )).getDay(),
                    hour: (new Date(Date.now() )).getHours(),
                    minute: (new Date(Date.now() )).getMinutes(),
                    endHour: new Date(Date.now() + 10*60000).getHours(),
                    endMinute: new Date(Date.now() + 10*60000).getMinutes(),
                    fertilizer: 1,
                }
            }]
            setData(dataObj)
        }
    }, [value,valueNumDays])
    React.useEffect(() => {
        LogBox.ignoreLogs(['VirtualizedLists should never be nested']);
    }, [])
    const getSelectedMode = () => {
        for(let key in selected){
            if(selected[key as keyof typeof selected]){
                return key
            }
        }
    }
    const submit = () => {
        dbRef = ref(db, `Pay${id}`)
        let error = null
        if(selected.EveryDay || selected.EveryNumDays){
            for(let i = 0; i < data.length; i++){
                error = timeError(data[i].data.hour, data[i].data.minute, data[i].data.endHour, data[i].data.endMinute,id,data, data[i].id)
                
            }
            if(error?.errorMessage !== '' || error?.timeError){
                alert(error?.errorMessage)
                return
            }

        }

        selected.EveryDay || selected.EveryNumDays
        ? selected.EveryDay? set(dbRef, {Mode: getSelectedMode(),data: data}):
        set(dbRef, {Mode: getSelectedMode(), data: data,numOfTimes: 1, startday: (new Date(Date.now() )).getDay(),Numofdays: valueNumDays})
        : set(dbRef, {Mode: getSelectedMode()})

        navigation.navigate('Home1', {id: 1})
    }
    

    return (
        <SafeAreaView style={styles.container}>
            <ScrollView >
                
            <View style={styles.checkboxContainer}>
              <Checkbox
                  style= {styles.checkbox}
                  value={selected["Automatic"]}
                  onValueChange={newValue => selectMode("Automatic")}   
              />
              <Text>{`Automatic`}</Text>
            </View>

            <View style={[styles.checkboxContainer, { zIndex: 20}]}>
              <Checkbox
                  style={[styles.checkbox, {paddingRight: 10}]}
                  value={selected["EveryDay"]}
                  onValueChange={newValue => selectMode("EveryDay")}
              />
              <Text style= {{  paddingRight: 10 }}>Every day</Text>
              <DropDownPicker
                    open={open}
                    value={value}
                    items={items}
                    setOpen={setOpen}
                    setValue={setValue}
                    setItems={setItems}
                    style={{width: 150}}
                    placeholder="Select number of times"
                />
                
            </View> 
            <View>
            {selected.EveryDay 
            &&
            <FlatList
        data={extractDataObj(Array.from(Array(value).keys()))}
        renderItem={renderItems }
        //keyExtractor={(item) => item.id.toString()}
      /> }
        
            </View>
            <View style={[styles.checkboxContainer, { zIndex: 10}]}>
              <Checkbox
                  style={[styles.checkbox, {paddingRight: 10}]}
                  value={selected["EveryNumDays"]}
                  onValueChange={newValue => selectMode("EveryNumDays")}
              />
              <Text style= {{  paddingRight: 10 }}>Every some days</Text>
              <DropDownPicker
                    open={openNumDAys}
                    value={valueNumDays}
                    items={itemsNumDays}
                    setOpen={setOpenNumDays}
                    setValue={setValueNumDays}
                    setItems={setItemsNumDays}
                    style={{width: 150}}
                    placeholder="Select number of days"
                />
                
            </View> 
            <View>
            {selected.EveryNumDays && <Picker id={0} 
            handleData={( index, elementData: any) => {
                    setData([{id: index, data: {...elementData, numOfDays: valueNumDays, day: (new Date(Date.now() )).getDay()}}])
            }}
            />}
            </View>

            <View style={styles.button}>
            <Button text={"Submit"} onPress={() => {submit()}}></Button>
            </View>
            

            </ScrollView>
        </SafeAreaView>
    )
}
export default Schedule