import React from "react";
import { View, Text, StyleSheet, TouchableOpacity } from "react-native";

const styles = StyleSheet.create({
    container: {
        flex: 1,
        justifyContent: "center",
        alignItems: "center",
        backgroundColor: "green",
        paddingVertical: 10,
        paddingHorizontal: 40,
        borderRadius: 20,
        height: 50,
        width: 200,
        margin: 5,
    },
    button: {
        height: 50,
    }
})
type ButtonProps = {
    text: string,
    onPress: () => void,
}
export const Button = ({text='click', onPress}:ButtonProps) => {
    return (
        <TouchableOpacity onPress={onPress}>
          <View style={styles.button}>
            <View style={styles.container}>
              <Text>{text}</Text>
            </View>
          </View>
        </TouchableOpacity>
    )
}