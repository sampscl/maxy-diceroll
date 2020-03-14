import React from 'react';
import _ from 'lodash';
import './dice.css';

const randomRoll = (max) => 1 + Math.floor(Math.random() * Math.floor(max));

class Die extends React.Component {
  constructor(props) {
    super(props);
    this.state = {value: randomRoll(props.sides), history: []};
  }

  reroll = () => {
    const { sides } = this.props;
    const { history } = this.state;

    const nextValue = randomRoll(sides);
    const updatedHistory = [...history, nextValue];
    this.setState({history: updatedHistory});
  }

  render() {
    const { sides } = this.props;
    const { history } = this.state;
    const historyString = _.join(history, ',');
    return <div className="die">
      <button className="rollButton" onClick={ this.reroll }>Roll d{ sides }</button>
      <button className="clearButton" onClick={ () => this.setState({history: []})}>X</button>
      <h3 className="historyText">{ historyString }</h3>
    </div>;
  }
}

export default class Dice extends React.Component {
  
  render() {
    return (<div className="container">
      <Die sides={20} />
      <Die sides={10} />
      <Die sides={8} />
      <Die sides={6} />
      <Die sides={4} />
    </div>);
  }

}
