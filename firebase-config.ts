// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
import { getDatabase, ref, set } from "firebase/database";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const startFirebase = () => {

const firebaseConfig = {
  apiKey: "AIzaSyDhrHsPBHnOoUqhRvLxbxsbslPRDxF9hyk",
  authDomain: "multi-pay-irrigation-system.firebaseapp.com",
  databaseURL: "https://multi-pay-irrigation-system-default-rtdb.firebaseio.com",
  projectId: "multi-pay-irrigation-system",
  storageBucket: "multi-pay-irrigation-system.appspot.com",
  messagingSenderId: "38853746218",
  appId: "1:38853746218:web:16dc4682acfb426c3aed53",
  measurementId: "G-L1TLRXDB9N"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
return getDatabase(app);

}
export default startFirebase;