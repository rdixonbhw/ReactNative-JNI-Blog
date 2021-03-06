/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 * @flow
 */

import React, { Component } from 'react';
import {
	AppRegistry,
	StyleSheet,
	Text,
	View
} from 'react-native';
import HelloWorld from './HelloWorld';

class blogRnJni extends Component {
	async helloWorld() {
		try {
			let struct = {
				vector_count: 3,
				vectors: [
					{
						x: 10,
						y: 10,
					},
					{
						x: 20,
						y: 20,
					},
					{
						x: 30,
						y: 30,
					},
				],
				position: {
					x: 12,
					y: 34,
				}
			};
			let helloWorldStr = await HelloWorld.helloWorld(struct, -3.14);
			console.log(helloWorldStr);
		} catch (e) {
			console.error(e);
		}
	}

	render() {
		this.helloWorld();
		return (
				<View style={styles.container}>
				<Text style={styles.welcome}>
				Welcome to React Native!
			</Text>
				<Text style={styles.instructions}>
				To get started, edit index.android.js
			</Text>
				<Text style={styles.instructions}>
				Double tap R on your keyboard to reload,{'\n'}
			Shake or press menu button for dev menu
			</Text>
				</View>
		);
	}
}

const styles = StyleSheet.create({
	container: {
		flex: 1,
		justifyContent: 'center',
		alignItems: 'center',
		backgroundColor: '#F5FCFF',
	},
	welcome: {
		fontSize: 20,
		textAlign: 'center',
		margin: 10,
	},
	instructions: {
		textAlign: 'center',
		color: '#333333',
		marginBottom: 5,
	},
});

AppRegistry.registerComponent('blogRnJni', () => blogRnJni);
