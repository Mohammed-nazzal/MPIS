// You can import Ionicons from @expo/vector-icons if you use Expo or
// react-native-vector-icons/Ionicons otherwise.
import * as React from 'react';
import { Text, View } from 'react-native';
import { Ionicons } from '@expo/vector-icons';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import { createStackNavigator } from '@react-navigation/stack';
import { MaterialCommunityIcons } from '@expo/vector-icons'; 

import { NavigationContainer } from '@react-navigation/native';
import Home from './screens/Home';
import Schedule from './screens/Schedule';

const Tab = createBottomTabNavigator();
const RootStack = createStackNavigator();
const TabNavigation = () => {
  return (<Tab.Navigator
    screenOptions={({ route }) => ({
      tabBarIcon: ({ focused, color, size }) => {
        let iconName;

        if (route.name === 'Home1') {
          iconName = 'home-floor-1'
        } else if (route.name === 'Home2') {
          iconName = 'home-floor-2'
        }

        // You can return any component that you like here!
        return <MaterialCommunityIcons name={iconName as any} size={size} color={color} />
      },
      tabBarActiveTintColor: 'tomato',
      tabBarInactiveTintColor: 'gray',
    })}
  >
    <Tab.Screen getId={() => "1"} name="Home1" component={Home} />
    <Tab.Screen getId={() => "2"} name="Home2" component={Home} />
  </Tab.Navigator>)}

export default function App() {
  return (
    <NavigationContainer>
      <RootStack.Navigator>
        <RootStack.Screen options={{headerShown: false}}   name="Home" component={TabNavigation} />
        <RootStack.Screen getId={() => "S1"} name="Schedule1" component={Schedule as any} />
        <RootStack.Screen getId={() => "S2"} name="Schedule2" component={Schedule as any} />
      </RootStack.Navigator>
      
    </NavigationContainer>
  )
}
