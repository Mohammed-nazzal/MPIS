import { onValue, ref } from "firebase/database";
import startFirebase from "../firebase-config";

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
export const timeError = (hour: number,minute:number,endHour: number, endMinute: number, pay:number, data: Array<Data>, index?: number) => {
    const database = startFirebase();
    const Pay1ref = ref(database, 'Pay1');
    const Pay2ref = ref(database, 'Pay2');
    let Pay1: Array<Data> = [];
    let Pay2: Array<Data> = [];
    onValue(Pay1ref, (snapshot) => {
        const data = snapshot.val();
        if(data.Mode === 'Automatic')
            Pay1= []
        else
        {Pay1 = data.data;}
        // console.log(data);
    });
    onValue(Pay2ref, (snapshot) => {
        const data = snapshot.val();
        if(data.Mode === 'Automatic')
          Pay2= []
        else
          {Pay2 = data.data;}
        // console.log(data.data);
    });
    
    let timeError = false;
    let errorMessage = '';
    if(pay === 1)
      Pay1 = data;
    for(let i of Pay1){
       //alert(i.id, index);
        if(i.id === index && pay === 1)
            continue;
        const payHour = (i.data.hour);
        const payMinute = (i.data.minute);
        const payHourEnd = (i.data.endHour);
        const payMinuteEnd = (i.data.endMinute);
        const period = (endHour*60 + endMinute) - (hour*60 + minute);
        if(hour*60 + minute > payHourEnd*60 + payMinuteEnd || hour*60 + minute + period < payHour*60 + payMinute){
            timeError = false;
        }
        else{
            timeError = true;
            errorMessage = 'Error: Time is overlapped with Pay1 in {time: ' + i.id+1 + '}'
        }
    }
    if(pay === 2)
      Pay2 = data;
    for(let i of Pay2){
        if(i.id === index && pay === 2)
            continue;
        const payHour = (i.data.hour);
        const payMinute = (i.data.minute);
        const payHourEnd = (i.data.endHour);
        const payMinuteEnd = (i.data.endMinute);
        const period = (endHour*60 + endMinute) - (hour*60 + minute);
        if(hour*60 + minute > payHourEnd*60 + payMinuteEnd + 1 || hour*60 + minute + period + 1 < payHour*60 + payMinute){
            timeError = false;
        }
        else{
            timeError = true;
            errorMessage = 'Error: Time is overlapped with Pay1 in {time: ' + i.id+1 + '}'
        }
    }
    return {timeError, errorMessage};
}
