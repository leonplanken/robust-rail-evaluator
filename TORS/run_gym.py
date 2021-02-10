from rl.tors_env import TORSEnv
from manager.config import Config
from stable_baselines.common.policies import MlpPolicy
from stable_baselines import PPO2

data_folder = "episode.json" 

config = Config.load_from_file(data_folder, "episode")
env = TORSEnv(config)
model = PPO2(MlpPolicy, env, verbose=1)
model.learn(total_timesteps=20000)

obs = env.reset()
for i in range(2000):
  action, _states = model.predict(obs)
  obs, rewards, done, info = env.step(action)
  env.render()
