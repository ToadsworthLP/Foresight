# Foresight

Foresight is a MIDI plugin that can offset MIDI notes by a configurable amount. This is useful when using sampled acoustic instruments, as the start of most audio samples are not what we perceive as the start of the note, or the transient. Foresight is especially useful when you want to be able to switch between multiple articulations in one track in your DAW, as Foresight can dynamically switch the MIDI note delay depending on what articulation is in use.

Note that currently Foresight is currently strictly monophonic. Notes in the specified playable range will cut each other off when more than one note is played at once.

## Configuration

```xml
<foresight version="4" name="NAME"></foresight>
```

Foresight's configuration uses XML. It must start with the version of the configuration format and the name of the configuration.

### Settings section (`<settings>`)

#### Range tag (`<range>`)

```xml
<range boundary="">note</range>
```

The `range` tag specifies what notes Foresight will process. Any notes outside of this range will be ignored and passed through as normal. The `lower` or `upper` boundaries together specifies a group of notes to process. For example, this will specify that only notes `A1-G5` should be processed:

```xml
<range boundary="lower">A1</range>
<range boundary="upper">G5</range>
```

#### Keyswitches tag (`<keyswitches>`)

```xml
<keyswitches boundary="">note</range>
```

The `keyswitches` tag operates exactly like the `range` tag, except it specifies what notes will be processed as keyswitches.

#### Block tag (`<block>`)

```xml
<block>value</block>
```

The `block` tag stops the specified CC or note (outside of what is specified in the `range` tags) from being passed to the output. This does not affect messages generated by nodes in the `output` section. A value starting with `CC` specifies a CC, anything else specifies a note, such as `C#3`.

```xml
<block>CC3</block>
<block>D#2</block>
```

### Input section (`<input>`)

The `input` section specifies the kinds of articulations this config will handle, and what triggers each articulation. `switch` nodes start a decision tree, and `case` nodes can match depending on the input. `switch` nodes are evaluated top to bottom, and multiple `case` nodes can be matched.

#### Switch node (`<switch>`)

```xml
<switch target="">articulation</switch>
```

`switch` nodes can have many different types of targets, and the value of the target is matched by `case` nodes:

- Starts with `KS_`:
    - Note that is inside of the `keyswitches` tags. Has no value.
    - Ex: `KS_C0`
- Starts with `CC`:
    - Value is the current value of the CC.
    - Ex: `CC1`
- Note name:
    - Held note outside of the `range` tags. Value is the note's velocity.
    - Ex: `G3`
- `legato`
    - Whether the current note is part of a legato phrase.
- `velocity`
    - The velocity of the current note.
- `length`
    - The length of the current note.
- `program`
    - The currently active MIDI program. Starts at 0, though some DAWs incorrectly report the first MIDI program as 1.

The articulation name specified here must have a corresponding `output` node as well.

#### Case node (`<case>`)

```xml
<case equals=""></case>
<case less=""></case>
<case greater=""></case>
<case less="" greater=""></case>
```

`case` nodes compare the target of the parent `switch` node to a value or range of values. All child nodes are ignored if the condition doesn't match.

- `equals` matches if the parent node's target matches the specified value.
- `less` matches if the parent node's target is less than the specified value.
- `greater` matches if the parent node's target is greater than the specified value.

### Output section (`<output>`)

The `output` section controls what happens when an articulation is matched by a `input` node.

#### Tag node (`<tag>`)

```xml
<tag name=""></tag>
```

`tag` nodes specify what articulation from the `input` section will be processed.

#### Set node (`<set>`)

```xml
<set target="">value</set>
```

`set` nodes specify what to do with a parent `tag` node's articulation. The `target` can be:

- `start`
    - Controls the amount of start delay on the current node, in milliseconds. Can be positive or negative.
    - Ex. `<set target="start">-150</set>`
- `end`
    - Controls the amount of ending delay on the current node, in milliseconds. Can be positive or negative.
    - Ex. `<set target="end">75</set>`
- `legato`
    - Allow the current articulation to start or be part of a legato phrase.
    - Ex. `<set target="legato"></set>`
- `note`
    - Play a new note.
    - Ex. `<set target="note">F3</set>`
- Starts with `CC`:
    - Send a CC message.
    - Ex. `<set target="CC11">127</set>`
- `program`
    - Change the active MIDI program.
    - Ex. `<set target="program">3</set>`

### Example configs

#### Orchestral Tools Berlin Orchestra Violins 1

```xml
<foresight version="4" name="OT BO Violins 1">
    <settings>
        <keyswitches boundary="lower">C1</keyswitches>
        <keyswitches boundary="upper">A1</keyswitches>
        <range boundary="lower">G2</range>
        <range boundary="upper">D6</range>
    </settings>
    <input>
        <switch target="KS_C1">
            <switch target="legato">
                <case equals="0">Sustain</case>
                <case equals="1">Legato</case>
            </switch>
        </switch>
        <switch target="KS_C#1">Sustain</switch>
        <switch target="KS_D1">Staccato</switch>
        <switch target="KS_E1">Spiccato</switch>
        <switch target="KS_F1">Pizzicato</switch>
        <switch target="KS_G1">Tremolo</switch>
        <switch target="KS_A1">Trill</switch>
    </input>
    <output>
        <tag name="Legato">
            <set target="legato" />
            <set target="start">-150</set>
        </tag>
        <tag name="Sustain">
            <set target="legato" />
            <set target="start">-50</set>
        </tag>
        <tag name="Staccato">
            <set target="start">-30</set>
        </tag>
        <tag name="Spiccato">
            <set target="start">-30</set>
        </tag>
        <tag name="Pizzicato">
            <set target="start">-20</set>
        </tag>
        <tag name="Tremolo">
            <set target="start">-140</set>
        </tag>
        <tag name="Trill">
            <set target="start">-80</set>
        </tag>
    </output>
</foresight>
```

#### Cinematic Studio Strings

```xml
<foresight version="4" name="Cinematic Studio Strings CC58">
    <settings>
        <range boundary="lower">C1</range>
        <range boundary="upper">C7</range>
        <block>CC58</block>
    </settings>
    <input>
        <switch target="CC58">
            <case greater="-1" less="6">
                <switch target="legato">
                    <case greater="0">
                        <switch target="velocity">
                            <case greater="-1" less="65">LowLatencySlowLegato</case>
                            <case greater="64" less="128">LowLatencyFastLegato</case>
                        </switch>
                    </case>
                    <case equals="0">LowLatencySustain</case>
                </switch>
            </case>
            <case greater="5" less="11">
                <switch target="legato">
                    <case greater="0">
                        <switch target="velocity">
                            <case greater="-1" less="65">SlowLegato</case>
                            <case greater="64" less="101">MediumLegato</case>
                            <case greater="100" less="128">FastLegato</case>
                        </switch>
                    </case>
                    <case equals="0">Sustain</case>
                </switch>
            </case>
            <case greater="10" less="16">Spiccato</case>
            <case greater="15" less="21">Staccatissimo</case>
            <case greater="20" less="26">Staccato</case>
            <case greater="25" less="31">Sforzando</case>
            <case greater="30" less="36">Pizzicato</case>
            <case greater="35" less="41">BartokSnap</case>
            <case greater="40" less="46">ColLegno</case>
            <case greater="45" less="51">Trills</case>
            <case greater="50" less="56">Harmonics</case>
            <case greater="55" less="61">Tremolo</case>
            <case greater="60" less="66">MeasuredTremolo</case>
            <case greater="65" less="71">MarcatoWithoutOverlay</case>
            <case greater="70" less="76">MarcatoWithOverlay</case>
        </switch>
    </input>
    <output>
        <tag name="LowLatencySustain">
            <set target="legato" />
            <set target="start">-50</set>
            <set target="CC58">0</set>
        </tag>
        <tag name="LowLatencySlowLegato">
            <set target="legato" />
            <set target="start">-150</set>
            <set target="CC58">0</set>
        </tag>
        <tag name="LowLatencyFastLegato">
            <set target="legato" />
            <set target="start">-50</set>
            <set target="CC58">0</set>
        </tag>
        <tag name="Sustain">
            <set target="legato" />
            <set target="start">-50</set>
            <set target="CC58">6</set>
        </tag>
        <tag name="SlowLegato">
            <set target="legato" />
            <set target="start">-333</set>
            <set target="end">-90</set>
            <set target="CC58">6</set>
        </tag>
        <tag name="MediumLegato">
            <set target="legato" />
            <set target="start">-250</set>
            <set target="CC58">6</set>
        </tag>
        <tag name="FastLegato">
            <set target="legato" />
            <set target="start">-110</set>
            <set target="CC58">6</set>
        </tag>
        <tag name="Spiccato">
            <set target="start">-60</set>
            <set target="CC58">11</set>
        </tag>
        <tag name="Staccatissimo">
            <set target="start">-60</set>
            <set target="CC58">16</set>
        </tag>
        <tag name="Staccato">
            <set target="start">-60</set>
            <set target="CC58">21</set>
        </tag>
        <tag name="Sforzando">
            <set target="start">-60</set>
            <set target="CC58">26</set>
        </tag>
        <tag name="Pizzicato">
            <set target="start">-60</set>
            <set target="CC58">31</set>
        </tag>
        <tag name="BartokSnap">
            <set target="start">-60</set>
            <set target="CC58">36</set>
        </tag>
        <tag name="ColLegno">
            <set target="start">-60</set>
            <set target="CC58">41</set>
        </tag>
        <tag name="Trills">
            <set target="start">-50</set>
            <set target="CC58">46</set>
        </tag>
        <tag name="Harmonics">
            <set target="start">-120</set>
            <set target="CC58">51</set>
        </tag>
        <tag name="Tremolo">
            <set target="start">-50</set>
            <set target="CC58">56</set>
        </tag>
        <tag name="MeasuredTremolo">
            <set target="start">-50</set>
            <set target="CC58">61</set>
        </tag>
        <tag name="MarcatoWithoutOverlay">
            <set target="start">-50</set>
            <set target="CC58">66</set>
        </tag>
        <tag name="MarcatoWithOverlay">
            <set target="start">-50</set>
            <set target="CC58">71</set>
        </tag>
    </output>
</foresight>
```
